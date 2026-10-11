// Ganondorf main-fragment views; the GameCube class leaves these fields unknown.
// HD: offsets follow the verified layout, preserving byte signedness and hair word copies.
#pragma once
#include "d/actor/d_a_gnd_exec.h"
union gndMainByte {
    be<u8> value;
    be<s8> signedValue;
};
WWHD_SIZE(gndMainByte, 1);
struct gnd_ke_s {
    cXyz mPos[20];
    cXyz mVelocity[20];
};
WWHD_SIZE(gnd_ke_s, 0x1E0);
WWHD_OFFSET(gnd_ke_s, mVelocity, 0xF0);
struct gndMainFields {
    u8 _pad0[0x3C8];
    /* 0x3C8 */ be<u32> mpSelf;
    u8 _pad3CC[0x1E];
    /* 0x3EA */ be<s16> mAction;
    /* 0x3EC */ be<s16> mMode;
    u8 _pad3EE[0x2];
    /* 0x3F0 */ cXyz mTargetPos;
    u8 _pad3FC[0x10];
    /* 0x40C */ be<f32> mTurnStep;
    /* 0x410 */ be<f32> mTurnBlend;
    /* 0x414 */ be<f32> mTargetSpeed;
    u8 _pad418[0x6];
    /* 0x41E */ be<s16> mActionTimer;
    /* 0x420 */ be<s16> mWaitTimer;
    u8 _pad422[0x6];
    /* 0x428 */ be<s16> mDamageTimer;
    /* 0x42A */ be<s16> mHitTimer;
    /* 0x42C */ be<f32> mKnockbackSpeed;
    /* 0x430 */ be<s16> mKnockbackAngle;
    u8 _pad432[0x6];
    /* 0x438 */ gndMainByte mLanded;
    u8 _pad439[0x94];
    /* 0x4CD */ be<u8> mParticleRed;
    u8 _pad4CE[0x1];
    /* 0x4CF */ be<u8> mParticleGreen;
    u8 _pad4D0[0x1];
    /* 0x4D1 */ be<u8> mParticleBlue;
    u8 _pad4D2[0x136];
    /* 0x608 */ be<u8> mSwordBlocked;
    /* 0x609 */ gndMainByte mGuardHits;
    /* 0x60A */ gndMainByte mGuardHit;
    /* 0x60B */ be<u8> mInvulnerabilityTimer;
    /* 0x60C */ gndMainByte mBattlePhase;
    /* 0x60D */ be<s8> mDamageRequest;
    /* 0x60E */ be<s8> mJumpTimer;
    /* 0x60F */ be<s8> mPreviousDodge;
    /* 0x610 */ be<s8> mDodge;
    u8 _pad611[0x1];
    /* 0x612 */ be<s16> mDodgeAngle;
    /* 0x614 */ be<f32> mModelYawOffset;
    /* 0x618 */ gnd_ke_s mHair[4];
    u8 _padD98[0x194];
    /* 0xF2C */ be<u8> mAcchFlags;
    u8 _padF2D[0x3];
    /* 0xF30 */ be<u32> mAcchVtable;
    /* 0xF34 */ be<u32> mPolyPassVtable;
    /* 0xF38 */ be<u8> mPolyPass;
    u8 _padF39[0x7];
    /* 0xF40 */ be<u32> mGroupPassVtable;
    u8 _padF44[0x7D4];
    /* 0x1718 */ be<u8> mSwordAttackLeft;
    /* 0x1719 */ be<u8> mSwordAttackRight;
    u8 _pad171A[0xE];
    /* 0x1728 */ be<f32> mSwordRadiusRight;
    u8 _pad172C[0x2];
    /* 0x172E */ gndMainByte mGuard;
    /* 0x172F */ be<u8> mGuardAttackType;
    /* 0x1730 */ be<s8> mGuardTimer;
    u8 _pad1731[0x3];
    /* 0x1734 */ be<u32> mAttackEmitters[6];
    u8 _pad174C[0xC];
    /* 0x1758 */ be<s16> mFlashTimers[15];
    u8 _pad1776[0x10];
    /* 0x1786 */ be<s16> mFlashColor;
    /* 0x1788 */ be<f32> mFlashFog;
    /* 0x178C */ be<s16> mEffectTimer;
    /* 0x178E */ be<s16> mFlashFrame;
    /* 0x1790 */ be<u32> mFlashEmitters[15];
    u8 _pad17CC[0x3C];
    /* 0x1808 */ cXyz mFlashOffsets[15];
    /* 0x18BC */ be<s16> mBlurRate;
    /* 0x18BE */ be<s16> mCameraCut;
    u8 _pad18C0[0x4C];
    /* 0x190C */ be<f32> mCameraShake;
};
WWHD_OFFSET(gndMainFields, mAction, 0x3EA);
WWHD_OFFSET(gndMainFields, mMode, 0x3EC);
WWHD_OFFSET(gndMainFields, mTargetPos, 0x3F0);
WWHD_OFFSET(gndMainFields, mTurnStep, 0x40C);
WWHD_OFFSET(gndMainFields, mTurnBlend, 0x410);
WWHD_OFFSET(gndMainFields, mTargetSpeed, 0x414);
WWHD_OFFSET(gndMainFields, mActionTimer, 0x41E);
WWHD_OFFSET(gndMainFields, mWaitTimer, 0x420);
WWHD_OFFSET(gndMainFields, mDamageTimer, 0x428);
WWHD_OFFSET(gndMainFields, mHitTimer, 0x42A);
WWHD_OFFSET(gndMainFields, mKnockbackSpeed, 0x42C);
WWHD_OFFSET(gndMainFields, mKnockbackAngle, 0x430);
WWHD_OFFSET(gndMainFields, mLanded, 0x438);
WWHD_OFFSET(gndMainFields, mParticleRed, 0x4CD);
WWHD_OFFSET(gndMainFields, mParticleGreen, 0x4CF);
WWHD_OFFSET(gndMainFields, mParticleBlue, 0x4D1);
WWHD_OFFSET(gndMainFields, mSwordBlocked, 0x608);
WWHD_OFFSET(gndMainFields, mGuardHits, 0x609);
WWHD_OFFSET(gndMainFields, mGuardHit, 0x60A);
WWHD_OFFSET(gndMainFields, mInvulnerabilityTimer, 0x60B);
WWHD_OFFSET(gndMainFields, mBattlePhase, 0x60C);
WWHD_OFFSET(gndMainFields, mDamageRequest, 0x60D);
WWHD_OFFSET(gndMainFields, mJumpTimer, 0x60E);
WWHD_OFFSET(gndMainFields, mPreviousDodge, 0x60F);
WWHD_OFFSET(gndMainFields, mDodge, 0x610);
WWHD_OFFSET(gndMainFields, mDodgeAngle, 0x612);
WWHD_OFFSET(gndMainFields, mModelYawOffset, 0x614);
WWHD_OFFSET(gndMainFields, mHair, 0x618);
WWHD_OFFSET(gndMainFields, mAcchFlags, 0xF2C);
WWHD_OFFSET(gndMainFields, mAcchVtable, 0xF30);
WWHD_OFFSET(gndMainFields, mPolyPassVtable, 0xF34);
WWHD_OFFSET(gndMainFields, mPolyPass, 0xF38);
WWHD_OFFSET(gndMainFields, mGroupPassVtable, 0xF40);
WWHD_OFFSET(gndMainFields, mSwordAttackLeft, 0x1718);
WWHD_OFFSET(gndMainFields, mSwordAttackRight, 0x1719);
WWHD_OFFSET(gndMainFields, mSwordRadiusRight, 0x1728);
WWHD_OFFSET(gndMainFields, mGuard, 0x172E);
WWHD_OFFSET(gndMainFields, mGuardAttackType, 0x172F);
WWHD_OFFSET(gndMainFields, mGuardTimer, 0x1730);
WWHD_OFFSET(gndMainFields, mAttackEmitters, 0x1734);
WWHD_OFFSET(gndMainFields, mFlashTimers, 0x1758);
WWHD_OFFSET(gndMainFields, mFlashColor, 0x1786);
WWHD_OFFSET(gndMainFields, mFlashFog, 0x1788);
WWHD_OFFSET(gndMainFields, mEffectTimer, 0x178C);
WWHD_OFFSET(gndMainFields, mFlashFrame, 0x178E);
WWHD_OFFSET(gndMainFields, mFlashEmitters, 0x1790);
WWHD_OFFSET(gndMainFields, mFlashOffsets, 0x1808);
WWHD_OFFSET(gndMainFields, mBlurRate, 0x18BC);
WWHD_OFFSET(gndMainFields, mCameraCut, 0x18BE);
WWHD_OFFSET(gndMainFields, mCameraShake, 0x190C);
WWHD_SIZE(gndMainFields, 0x1910);
inline gndMainFields* mainFields(gnd_class* actor) {
    return gabi::at<gndMainFields>(gabi::ea(actor));
}

// HD: named bindings retain the original call-site register widths.
void gndMain_acchDestruct(dBgS_Acch* acch, s32 flags);
void gndMain_circleDestruct(void* circle, s32 flags);
void gndMain_actorDestruct(gnd_class* actor, s32 flags);
void gndMain_acchConstruct(dBgS_Acch* acch);
void gndMain_globalByteConstruct(s32 address);
void gndMain_globalByteDestruct(s32 address);
void gndMain_execute(gnd_class* actor);
void gndMain_monsSeStart(u32 id, cXyz* pos, u32 actorID, s32 param, s32 reverb);
BOOL gndMain_startShock(u32 vibration, s32 strength, s32 length, cXyz* pos);
void gndMain_report(s32 x, s32 y, u32 fmt, s32 value);

// HD: these three compact initialization records occupy parts of the battle lighting state.
struct gndInitRecord {
    be<f32> mPositionValues[6];
    be<u8> mColorValues[4];
    be<s16> mAngleValues[4];
    be<f32> mLightValues[8];
};
WWHD_SIZE(gndInitRecord, 0x44);
WWHD_OFFSET(gndInitRecord, mColorValues, 0x18);
WWHD_OFFSET(gndInitRecord, mAngleValues, 0x1C);
WWHD_OFFSET(gndInitRecord, mLightValues, 0x24);
struct gndInitFields {
    u8 _padStart[0x43C];
    gndInitRecord mBattle;
    u8 _padBattle[0x4FC - 0x480];
    gndInitRecord mSword;
    u8 _padSword[0x580 - 0x540];
    gndInitRecord mBody;
};
WWHD_OFFSET(gndInitFields, mBattle, 0x43C);
WWHD_OFFSET(gndInitFields, mSword, 0x4FC);
WWHD_OFFSET(gndInitFields, mBody, 0x580);
WWHD_SIZE(gndInitFields, 0x5C4);
inline gndInitFields* gndInitialFields(gnd_class* actor) {
    return gabi::at<gndInitFields>(gabi::ea(actor));
}

struct gndMainHIO {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ be<u8> mFreeze;
    /* 0x2 */ be<u8> mDebug;
    /* 0x3 */ be<u8> mDrawHair;
    /* 0x4 */ be<f32> mHairLength;
    /* 0x8 */ be<f32> mScale;
    /* 0xC */ be<u8> mPhaseOverride;
    /* 0xD */ be<u8> mPhaseLock;
    /* 0xE */ be<u8> mJumpType;
    u8 _padF[1];
    /* 0x10 */ be<f32> mMoveRate;
    /* 0x14 */ be<f32> mFollowDistance;
    /* 0x18 */ be<f32> mAttackDistance;
    /* 0x1C */ be<f32> mJumpSpeed;
    /* 0x20 */ be<f32> mWalkSpeed;
    /* 0x24 */ be<f32> mRetreatSpeed;
    /* 0x28 */ be<f32> mGuardDistance;
    /* 0x2C */ be<f32> mFallSpeed;
    /* 0x30 */ be<f32> mJumpHeight;
    /* 0x34 */ be<f32> mAirGravity;
    /* 0x38 */ be<f32> mCloseAttackDistance;
    /* 0x3C */ be<f32> mDodgeDistance;
    /* 0x40 */ be<u8> mGuardMode;
    /* 0x41 */ be<u8> mSwordShield;
    /* 0x42 */ be<u8> mJumpEnabled;
    u8 _pad43[1];
    /* 0x44 */ be<f32> mHairAngle;
    /* 0x48 */ be<f32> mCameraOffset;
    /* 0x4C */ be<f32> mTurnRate;
    /* 0x50 */ be<f32> mDodgeJumpSpeed;
    /* 0x54 */ be<f32> mDodgeYaw;
    /* 0x58 */ be<f32> mDodgeHeight;
    /* 0x5C */ be<f32> mDodgeGravity;
    /* 0x60 */ be<f32> mDodgeStep;
    /* 0x64 */ be<s16> mAttackWait;
    /* 0x66 */ be<s16> mAttackWaitAlt;
    /* 0x68 */ be<s16> mAttackDelay;
    /* 0x6A */ be<s16> mCounterWait;
    /* 0x6C */ be<s16> mParryWait;
    /* 0x6E */ be<s16> mParryWaitAlt;
    /* 0x70 */ be<s16> mAttackStart;
    /* 0x72 */ be<s16> mAttackEnd;
    /* 0x74 */ be<f32> mRecoveryDistance;
    /* 0x78 */ be<s16> mFinishStart;
    /* 0x7A */ be<s16> mFinishActive;
    /* 0x7C */ be<s16> mFinishEnd;
    /* 0x7E */ be<s16> mSwordAttackType;
    /* 0x80 */ be<s16> mSpinAttackType;
    /* 0x82 */ be<s16> mDamageWait;
    /* 0x84 */ be<u8> mLongIntro;
    /* 0x85 */ be<u8> mStartZeldaDemo;
    u8 _pad86[2];
    /* 0x88 */ be<u32> __vtbl;
};
WWHD_SIZE(gndMainHIO, 0x8C);
WWHD_OFFSET(gndMainHIO, mNo, 0x0);
WWHD_OFFSET(gndMainHIO, mFreeze, 0x1);
WWHD_OFFSET(gndMainHIO, mDebug, 0x2);
WWHD_OFFSET(gndMainHIO, mDrawHair, 0x3);
WWHD_OFFSET(gndMainHIO, mHairLength, 0x4);
WWHD_OFFSET(gndMainHIO, mScale, 0x8);
WWHD_OFFSET(gndMainHIO, mPhaseOverride, 0xC);
WWHD_OFFSET(gndMainHIO, mPhaseLock, 0xD);
WWHD_OFFSET(gndMainHIO, mJumpType, 0xE);
WWHD_OFFSET(gndMainHIO, mMoveRate, 0x10);
WWHD_OFFSET(gndMainHIO, mFollowDistance, 0x14);
WWHD_OFFSET(gndMainHIO, mAttackDistance, 0x18);
WWHD_OFFSET(gndMainHIO, mJumpSpeed, 0x1C);
WWHD_OFFSET(gndMainHIO, mWalkSpeed, 0x20);
WWHD_OFFSET(gndMainHIO, mRetreatSpeed, 0x24);
WWHD_OFFSET(gndMainHIO, mGuardDistance, 0x28);
WWHD_OFFSET(gndMainHIO, mFallSpeed, 0x2C);
WWHD_OFFSET(gndMainHIO, mJumpHeight, 0x30);
WWHD_OFFSET(gndMainHIO, mAirGravity, 0x34);
WWHD_OFFSET(gndMainHIO, mCloseAttackDistance, 0x38);
WWHD_OFFSET(gndMainHIO, mDodgeDistance, 0x3C);
WWHD_OFFSET(gndMainHIO, mGuardMode, 0x40);
WWHD_OFFSET(gndMainHIO, mSwordShield, 0x41);
WWHD_OFFSET(gndMainHIO, mJumpEnabled, 0x42);
WWHD_OFFSET(gndMainHIO, mHairAngle, 0x44);
WWHD_OFFSET(gndMainHIO, mCameraOffset, 0x48);
WWHD_OFFSET(gndMainHIO, mTurnRate, 0x4C);
WWHD_OFFSET(gndMainHIO, mDodgeJumpSpeed, 0x50);
WWHD_OFFSET(gndMainHIO, mDodgeYaw, 0x54);
WWHD_OFFSET(gndMainHIO, mDodgeHeight, 0x58);
WWHD_OFFSET(gndMainHIO, mDodgeGravity, 0x5C);
WWHD_OFFSET(gndMainHIO, mDodgeStep, 0x60);
WWHD_OFFSET(gndMainHIO, mAttackWait, 0x64);
WWHD_OFFSET(gndMainHIO, mAttackWaitAlt, 0x66);
WWHD_OFFSET(gndMainHIO, mAttackDelay, 0x68);
WWHD_OFFSET(gndMainHIO, mCounterWait, 0x6A);
WWHD_OFFSET(gndMainHIO, mParryWait, 0x6C);
WWHD_OFFSET(gndMainHIO, mParryWaitAlt, 0x6E);
WWHD_OFFSET(gndMainHIO, mAttackStart, 0x70);
WWHD_OFFSET(gndMainHIO, mAttackEnd, 0x72);
WWHD_OFFSET(gndMainHIO, mRecoveryDistance, 0x74);
WWHD_OFFSET(gndMainHIO, mFinishStart, 0x78);
WWHD_OFFSET(gndMainHIO, mFinishActive, 0x7A);
WWHD_OFFSET(gndMainHIO, mFinishEnd, 0x7C);
WWHD_OFFSET(gndMainHIO, mSwordAttackType, 0x7E);
WWHD_OFFSET(gndMainHIO, mSpinAttackType, 0x80);
WWHD_OFFSET(gndMainHIO, mDamageWait, 0x82);
WWHD_OFFSET(gndMainHIO, mLongIntro, 0x84);
WWHD_OFFSET(gndMainHIO, mStartZeldaDemo, 0x85);
WWHD_OFFSET(gndMainHIO, __vtbl, 0x88);
inline gndMainHIO* mainHIO() { return gabi::at<gndMainHIO>(L_HIO); }

#include "d/actor/d_a_player.h"
struct gndMainEmitter {
    u8 _pad0[0x5C];
    be<s32> mMaxFrame;
    u8 _pad60[0x220 - 0x60];
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mParticleScale;
    GXColor mGlobalColorPrm;
    u8 _pad248[0x254 - 0x248];
    be<u32> mStatus;
    u8 _pad258[10];
    be<u8> mDrawType;
    u8 _pad263;
};
WWHD_SIZE(gndMainEmitter, 0x264);
WWHD_OFFSET(gndMainEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(gndMainEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(gndMainEmitter, mParticleScale, 0x238);
WWHD_OFFSET(gndMainEmitter, mGlobalColorPrm, 0x244);
WWHD_OFFSET(gndMainEmitter, mStatus, 0x254);
WWHD_OFFSET(gndMainEmitter, mDrawType, 0x262);
// HD: effect setup writes guest emitter fields even when the returned address is zero.
inline gndMainEmitter* mainEmitter(u32 address) {
    return reinterpret_cast<gndMainEmitter*>(PPC_MEM_BASE + address);
}
struct gndMainArrow {
    u8 _padStart[0x780];
    be<u8> mReflected;
};
WWHD_SIZE(gndMainArrow, 0x781);
struct gndMainPlayerState {
    u8 _padStart[0x69E8];
    be<u8> mSwordAttack;
};
WWHD_SIZE(gndMainPlayerState, 0x69E9);
// HD: the shared player cut byte is a plain SDK byte; this view uses guest-backed storage.
struct gndMainPlayer {
    u8 _padStart[0x3AC];
    be<u8> mCutType;
    u8 _padCut[15];
    be<u32> mNoResetFlg1;
};
WWHD_OFFSET(gndMainPlayer, mCutType, 0x3AC);
WWHD_OFFSET(gndMainPlayer, mNoResetFlg1, 0x3BC);
WWHD_SIZE(gndMainPlayer, 0x3C0);
inline gndMainPlayer* mainPlayer(fopAc_ac_c* player) {
    return reinterpret_cast<gndMainPlayer*>(PPC_MEM_BASE + gabi::ea(player));
}
struct gndMainLineData {
    be<u32> mpPos;
    u8 _padPos[12];
};
WWHD_SIZE(gndMainLineData, 16);
struct gndMainLineMat {
    u8 _padStart[0x144];
    gptr<gndMainLineData> mpLines;
};
WWHD_SIZE(gndMainLineMat, 0x148);
struct gndMainWordVector {
    be<u32> words[3];
};
WWHD_SIZE(gndMainWordVector, 12);
inline gndMainWordVector* mainWordVectors(u32 address) {
    return reinterpret_cast<gndMainWordVector*>(PPC_MEM_BASE + address);
}

// HD: aliases of the TU objects already mapped in the execute fragment, with no new definitions.
struct gndMainStatics {
    be<u32> mpZelda;
    be<f32> mAngleMin, mAngleMax;
    be<u32> mRippleScaleInitialized;
    be<u8> mConstructedByte, mDestructedByte;
    u8 _padBytes[2];
    be<u32> mZeroWords[4];
    cXyz mInactiveSwordPos;
    u8 _padSword[12];
    be<f32> mCameraCenters[15];
    be<f32> mCameraEyes[15];
};
WWHD_OFFSET(gndMainStatics, mZeroWords, 0x14);
WWHD_OFFSET(gndMainStatics, mInactiveSwordPos, 0x24);
WWHD_OFFSET(gndMainStatics, mCameraCenters, 0x3C);
WWHD_OFFSET(gndMainStatics, mCameraEyes, 0x78);
WWHD_SIZE(gndMainStatics, 0xB4);
inline gndMainStatics* mainStatics() { return gabi::at<gndMainStatics>(0x10464008); }
struct gndMainOpaque {
    u8 byte;
};
WWHD_SIZE(gndMainOpaque, 1);
using gndMainFlag = be<u8>;
inline be<u32>& mainDemoFrame() { return *gabi::at<be<u32>>(0x101D600C); }

// Entry points retained by the main fragment.
BOOL checkGround(gnd_class* i_this, f32 y);
void splash_set(gnd_class* i_this);
void attack_eff_remove(gnd_class* i_this);
void anm_init(gnd_class* i_this, int anm, f32 morf, u8 mode, f32 speed, int bas);
void* z_s_sub(void* a, void*);
BOOL player_view_check(gnd_class* i_this, s16 angle);
void pos_move(gnd_class* i_this, s8 noTurn);
void wait_set(gnd_class* i_this);
void* shot_s_sub(void* a, void*);
BOOL daGnd_IsDelete(gnd_class*);
BOOL daGnd_Delete(gnd_class* i_this);
void gnd_SafeString_dt(void* str, s32 flags);
void demowait(gnd_class* i_this);
void yawait(gnd_class* i_this);
void gnd_SafeString_assureTermination(void*);
void gnd_class_dt(gnd_class* i_this, s32 flags);
gnd_class* gnd_class_ct(gnd_class* i_this);
void* daGnd_HIO_ct(void* hio);
void __sinit_d_a_gnd_cpp();
BOOL useHeapInit(fopAc_ac_c* a_this);
cPhs_State daGnd_Create(fopAc_ac_c* a_this);
BOOL daGnd_Draw(gnd_class* i_this);
void finish(gnd_class* i_this);
void attack_last(gnd_class* i_this);
void body_flash(gnd_class* i_this);
void ke_move(gnd_class* i_this);
void damage(gnd_class* i_this);
void damage_check(gnd_class* i_this);
void defence0(gnd_class* i_this);

enum gndResourceID {
    GND_MODEL = 0x5C,
    GND_INITIAL_ANIMATION = 0x55,
    GND_COLOR_ANIMATION = 0x5F,
    GND_TEXTURE_ANIMATION = 0x63,
    GND_PATTERN_ANIMATION = 0x66
};

WWHD_OFFSET(gndMainFields, mpSelf, 0x3C8);
