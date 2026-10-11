/** Ganondorf movement layouts.
 * HD: actor fields use the expanded HD layout; only the verified movement view is exposed.
 * mNextParryOpeningType moves from GameCube 0x13CF to HD 0x172F (+0x360).
 * The GameCube declaration supplies mNextParryOpeningType; other movement fields are named by use.
 */
#pragma once
#include "d/actor/d_a_gnd.h"

struct GndMoveStateHD {
    u8 mPadding0[0x3EA];
    /* 0x03EA */ be<s16> mAction;
    /* 0x03EC */ be<s16> mMode;
    u8 mPadding3EE[0x2];
    /* 0x03F0 */ cXyz mTargetPos;
    /* 0x03FC */ cXyz mSavedSpeed;
    u8 mPadding408[0x4];
    /* 0x040C */ be<f32> mTurnRate;
    u8 mPadding410[0x4];
    /* 0x0414 */ be<f32> mTargetSpeed;
    u8 mPadding418[0x6];
    /* 0x041E */ be<s16> mActionTimer;
    /* 0x0420 */ be<s16> mAttackCooldown;
    u8 mPadding422[0x6];
    /* 0x0428 */ be<s16> mDefenceTimer;
    /* 0x042A */ be<s16> mDamageTimer;
    /* 0x042C */ be<f32> mKnockbackSpeed;
    /* 0x0430 */ be<s16> mKnockbackAngle;
    u8 mPadding432[0x4];
    /* 0x0436 */ be<s16> mZeldaAngle;
    u8 mPadding438[0x95];
    /* 0x04CD */ be<u8> mEffectRed;
    u8 mPadding4CE[0x1];
    /* 0x04CF */ be<u8> mEffectGreen;
    u8 mPadding4D0[0x1];
    /* 0x04D1 */ be<u8> mEffectBlue;
    u8 mPadding4D2[0x136];
    /* 0x0608 */ be<s8> mCounterRequest;
    /* 0x0609 */ be<u8> mDefenceChoice;
    u8 mPadding60A[0x1];
    /* 0x060B */ be<u8> mCombatEffect;
    /* 0x060C */ be<s8> mFightPhase;
    u8 mPadding60D[0x2];
    /* 0x060F */ be<u8> mDefenceRequest;
    /* 0x0610 */ be<u8> mDefenceCount;
    u8 mPadding611[0x1107];
    /* 0x1718 */ be<u8> mLeftSwordAttack;
    /* 0x1719 */ be<u8> mRightSwordAttack;
    u8 mPadding171A[0xA];
    /* 0x1724 */ be<f32> mLeftSwordRadius;
    /* 0x1728 */ be<f32> mRightSwordRadius;
    u8 mPadding172C[0x2];
    /* 0x172E */ be<u8> mCheckLightArrows;
    /* 0x172F */ be<u8> mNextParryOpeningType;
    u8 mPadding1730[0x1];
    /* 0x1731 */ be<s8> mImpactTimer;
    u8 mPadding1732[0x1A];
    /* 0x174C */ be<u32> mAttackEmitter;
    u8 mPadding1750[0x16C];
    /* 0x18BC */ be<s16> mZeldaTimer;
    /* 0x18BE */ be<s16> mDemoMode;
    /* 0x18C0 */ be<s16> mDemoTimer;
    u8 mPadding18C2[0x7E];
    /* 0x1940 */ cXyz mArrowCheckPos;
    u8 mTail[0x4];
};
WWHD_OFFSET(GndMoveStateHD, mAction, 0x3EA);
WWHD_OFFSET(GndMoveStateHD, mMode, 0x3EC);
WWHD_OFFSET(GndMoveStateHD, mTargetPos, 0x3F0);
WWHD_OFFSET(GndMoveStateHD, mSavedSpeed, 0x3FC);
WWHD_OFFSET(GndMoveStateHD, mTurnRate, 0x40C);
WWHD_OFFSET(GndMoveStateHD, mTargetSpeed, 0x414);
WWHD_OFFSET(GndMoveStateHD, mActionTimer, 0x41E);
WWHD_OFFSET(GndMoveStateHD, mAttackCooldown, 0x420);
WWHD_OFFSET(GndMoveStateHD, mDefenceTimer, 0x428);
WWHD_OFFSET(GndMoveStateHD, mDamageTimer, 0x42A);
WWHD_OFFSET(GndMoveStateHD, mKnockbackSpeed, 0x42C);
WWHD_OFFSET(GndMoveStateHD, mKnockbackAngle, 0x430);
WWHD_OFFSET(GndMoveStateHD, mZeldaAngle, 0x436);
WWHD_OFFSET(GndMoveStateHD, mEffectRed, 0x4CD);
WWHD_OFFSET(GndMoveStateHD, mEffectGreen, 0x4CF);
WWHD_OFFSET(GndMoveStateHD, mEffectBlue, 0x4D1);
WWHD_OFFSET(GndMoveStateHD, mCounterRequest, 0x608);
WWHD_OFFSET(GndMoveStateHD, mDefenceChoice, 0x609);
WWHD_OFFSET(GndMoveStateHD, mCombatEffect, 0x60B);
WWHD_OFFSET(GndMoveStateHD, mFightPhase, 0x60C);
WWHD_OFFSET(GndMoveStateHD, mDefenceRequest, 0x60F);
WWHD_OFFSET(GndMoveStateHD, mDefenceCount, 0x610);
WWHD_OFFSET(GndMoveStateHD, mLeftSwordAttack, 0x1718);
WWHD_OFFSET(GndMoveStateHD, mRightSwordAttack, 0x1719);
WWHD_OFFSET(GndMoveStateHD, mLeftSwordRadius, 0x1724);
WWHD_OFFSET(GndMoveStateHD, mRightSwordRadius, 0x1728);
WWHD_OFFSET(GndMoveStateHD, mCheckLightArrows, 0x172E);
WWHD_OFFSET(GndMoveStateHD, mNextParryOpeningType, 0x172F);
WWHD_OFFSET(GndMoveStateHD, mImpactTimer, 0x1731);
WWHD_OFFSET(GndMoveStateHD, mAttackEmitter, 0x174C);
WWHD_OFFSET(GndMoveStateHD, mZeldaTimer, 0x18BC);
WWHD_OFFSET(GndMoveStateHD, mDemoMode, 0x18BE);
WWHD_OFFSET(GndMoveStateHD, mDemoTimer, 0x18C0);
WWHD_OFFSET(GndMoveStateHD, mArrowCheckPos, 0x1940);
WWHD_SIZE(GndMoveStateHD, 0x1950);

struct GndMoveHioHD {
    u8 mPadding0[0xD];
    /* 0x000D */ be<u8> mComboChoice;
    u8 mPaddingE[0x2];
    /* 0x0010 */ be<f32> mMoveSpeed;
    /* 0x0014 */ be<f32> mNearDistance;
    /* 0x0018 */ be<f32> mFarDistance;
    /* 0x001C */ be<f32> mJumpSpeed;
    /* 0x0020 */ be<f32> mJumpY;
    /* 0x0024 */ be<f32> mJumpGravity;
    /* 0x0028 */ be<f32> mCloseAttackDistance;
    /* 0x002C */ be<f32> mBackJumpSpeed;
    /* 0x0030 */ be<f32> mBackJumpY;
    /* 0x0034 */ be<f32> mBackJumpGravity;
    /* 0x0038 */ be<f32> mComboDistance;
    /* 0x003C */ be<f32> mDefenceDistance;
    /* 0x0040 */ be<u8> mDisableGuard;
    u8 mPadding41[0x3];
    /* 0x0044 */ be<f32> mAlternateAttackChance;
    /* 0x0048 */ be<f32> mComboAdvanceSpeed;
    /* 0x004C */ be<f32> mComboSwingSpeed;
    /* 0x0050 */ be<f32> mComboWindupSpeed;
    u8 mPadding54[0x4];
    /* 0x0058 */ be<f32> mAttackJumpY;
    /* 0x005C */ be<f32> mAttackGravity;
    /* 0x0060 */ be<f32> mCounterSpeed;
    /* 0x0064 */ be<s16> mAttackCooldown;
    /* 0x0066 */ be<s16> mFirstParryFrame;
    /* 0x0068 */ be<s16> mComboParryStart;
    /* 0x006A */ be<s16> mComboParryEnd;
    /* 0x006C */ be<s16> mJumpParryFrame;
    /* 0x006E */ be<s16> mLandingParryStart;
    /* 0x0070 */ be<s16> mLandingParryEnd;
    /* 0x0072 */ be<s16> mCounterParryFrame;
    u8 mTail[0x18];
};
WWHD_OFFSET(GndMoveHioHD, mComboChoice, 0xD);
WWHD_OFFSET(GndMoveHioHD, mMoveSpeed, 0x10);
WWHD_OFFSET(GndMoveHioHD, mNearDistance, 0x14);
WWHD_OFFSET(GndMoveHioHD, mFarDistance, 0x18);
WWHD_OFFSET(GndMoveHioHD, mJumpSpeed, 0x1C);
WWHD_OFFSET(GndMoveHioHD, mJumpY, 0x20);
WWHD_OFFSET(GndMoveHioHD, mJumpGravity, 0x24);
WWHD_OFFSET(GndMoveHioHD, mCloseAttackDistance, 0x28);
WWHD_OFFSET(GndMoveHioHD, mBackJumpSpeed, 0x2C);
WWHD_OFFSET(GndMoveHioHD, mBackJumpY, 0x30);
WWHD_OFFSET(GndMoveHioHD, mBackJumpGravity, 0x34);
WWHD_OFFSET(GndMoveHioHD, mComboDistance, 0x38);
WWHD_OFFSET(GndMoveHioHD, mDefenceDistance, 0x3C);
WWHD_OFFSET(GndMoveHioHD, mDisableGuard, 0x40);
WWHD_OFFSET(GndMoveHioHD, mAlternateAttackChance, 0x44);
WWHD_OFFSET(GndMoveHioHD, mComboAdvanceSpeed, 0x48);
WWHD_OFFSET(GndMoveHioHD, mComboSwingSpeed, 0x4C);
WWHD_OFFSET(GndMoveHioHD, mComboWindupSpeed, 0x50);
WWHD_OFFSET(GndMoveHioHD, mAttackJumpY, 0x58);
WWHD_OFFSET(GndMoveHioHD, mAttackGravity, 0x5C);
WWHD_OFFSET(GndMoveHioHD, mCounterSpeed, 0x60);
WWHD_OFFSET(GndMoveHioHD, mAttackCooldown, 0x64);
WWHD_OFFSET(GndMoveHioHD, mFirstParryFrame, 0x66);
WWHD_OFFSET(GndMoveHioHD, mComboParryStart, 0x68);
WWHD_OFFSET(GndMoveHioHD, mComboParryEnd, 0x6A);
WWHD_OFFSET(GndMoveHioHD, mJumpParryFrame, 0x6C);
WWHD_OFFSET(GndMoveHioHD, mLandingParryStart, 0x6E);
WWHD_OFFSET(GndMoveHioHD, mLandingParryEnd, 0x70);
WWHD_OFFSET(GndMoveHioHD, mCounterParryFrame, 0x72);
WWHD_SIZE(GndMoveHioHD, 0x8C);

inline GndMoveStateHD& moveState(gnd_class* actor) { return *reinterpret_cast<GndMoveStateHD*>(actor); }
inline be<u32>& moveWord(be<f32>& value) { return *reinterpret_cast<be<u32>*>(&value); }

struct GndMoveActorFlagsHD {
    u8 mPadding[0x3AC];
    be<u8> mArrowState;
};
WWHD_OFFSET(GndMoveActorFlagsHD, mArrowState, 0x3AC);
WWHD_SIZE(GndMoveActorFlagsHD, 0x3AD);
struct GndMovePlayerHD {
    u8 mPadding[0x3B0];
    be<s16> mCounterState;
    u8 mPaddingCounter[0x6A70 - 0x3B2];
    be<u32> mStatus;
};
WWHD_OFFSET(GndMovePlayerHD, mCounterState, 0x3B0);
WWHD_OFFSET(GndMovePlayerHD, mStatus, 0x6A70);
WWHD_SIZE(GndMovePlayerHD, 0x6A74);
struct GndMovePlayHD {
    u8 mPadding[0x5B2C];
    gptr<fopAc_ac_c> mpPlayer;
    u8 mPaddingPlayer[4];
    gptr<GndMovePlayerHD> mpLink;
};
WWHD_OFFSET(GndMovePlayHD, mpPlayer, 0x5B2C);
WWHD_OFFSET(GndMovePlayHD, mpLink, 0x5B34);
WWHD_SIZE(GndMovePlayHD, 0x5B38);
struct GndMoveZeldaHD {
    u8 mPadding[0x857];
    be<u8> mFightRequest;
    u8 mPaddingRequest[0xADC - 0x858];
    be<u32> mHitRequest;
};
WWHD_OFFSET(GndMoveZeldaHD, mFightRequest, 0x857);
WWHD_OFFSET(GndMoveZeldaHD, mHitRequest, 0xADC);
WWHD_SIZE(GndMoveZeldaHD, 0xAE0);
struct GndMoveEmitterHD {
    u8 mPadding[0x1F0];
    Mtx34 mRotation;
    u8 mPaddingRotation[0x22C - 0x220];
    cXyz mTranslation;
    u8 mPaddingTranslation[0x244 - 0x238];
    be<u8> mRed;
    be<u8> mGreen;
    be<u8> mBlue;
};
WWHD_OFFSET(GndMoveEmitterHD, mRotation, 0x1F0);
WWHD_OFFSET(GndMoveEmitterHD, mTranslation, 0x22C);
WWHD_OFFSET(GndMoveEmitterHD, mRed, 0x244);
WWHD_OFFSET(GndMoveEmitterHD, mGreen, 0x245);
WWHD_OFFSET(GndMoveEmitterHD, mBlue, 0x246);
WWHD_SIZE(GndMoveEmitterHD, 0x247);
void gndMoveMonsterSound(u32, cXyz*, u32, u32, s32);
void gndMoveChangeBgmStatus(s32);
void gndMoveEmitterTransform(Mtx34*, Mtx34*, cXyz*);

enum GndMoveAction {
    GND_ACTION_MOVE = 0,
    GND_ACTION_ATTACK_COMBO = 1,
    GND_ACTION_ATTACK_ALTERNATE = 2,
    GND_ACTION_ATTACK_QUICK = 3,
    GND_ACTION_ATTACK_ZELDA = 4,
    GND_ACTION_ATTACK_LAST = 5,
    GND_ACTION_DEFENCE = 10,
    GND_ACTION_DAMAGE = 11,
    GND_ACTION_DEMO_WAIT = 20,
    GND_ACTION_YA_WAIT = 21,
    GND_ACTION_FINISH = 30,
};
enum GndMoveAnimation {
    GND_BCK_COMBO_LEFT_START = 0x24,
    GND_BCK_COMBO_LEFT_FOLLOW = 0x25,
    GND_BCK_COMBO_LEFT_FINISH = 0x26,
    GND_BCK_COMBO_RIGHT_START = 0x27,
    GND_BCK_COMBO_RIGHT_FOLLOW = 0x28,
    GND_BCK_JUMP_SLASH_START = 0x29,
    GND_BCK_JUMP_SLASH_LAND = 0x2A,
    GND_BCK_COMBO_RIGHT_FINISH = 0x2B,
    GND_BCK_ZELDA_STRIKE_START = 0x2C,
    GND_BCK_ZELDA_STRIKE_FINISH = 0x2D,
    GND_BCK_QUICK_LEFT = 0x36,
    GND_BCK_QUICK_RIGHT = 0x37,
    GND_BCK_TURN = 0x3F,
    GND_BCK_FALL = 0x41,
    GND_BCK_LAND = 0x42,
    GND_BCK_JUMP_LOOP = 0x43,
    GND_BCK_JUMP_START = 0x44,
    GND_BCK_CHARGE_FALL = 0x45,
    GND_BCK_CHARGE_LAND = 0x46,
    GND_BCK_CHARGE_JUMP = 0x47,
    GND_BCK_CHARGE_START = 0x48,
    GND_BCK_BACK_JUMP_START = 0x4A,
    GND_BCK_BACK_JUMP = 0x4B,
    GND_BCK_BACK_JUMP_LAND = 0x4C,
    GND_BCK_COUNTER_START = 0x4D,
    GND_BCK_COUNTER_SLASH = 0x4E,
    GND_BCK_ZELDA_WAIT = 0x4F,
    GND_BCK_COUNTER_RECOVER = 0x50,
    GND_BCK_ALTERNATE_LEFT = 0x53,
    GND_BCK_ALTERNATE_RIGHT = 0x54,
    GND_BCK_WAIT = 0x58,
    GND_BCK_MOVE = 0x59,
};
