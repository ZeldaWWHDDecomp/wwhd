/* Rat shopkeeper, WWHD layout. Local game- */
#pragma once
#include "d/actor/d_a_npc_ba1.h"

struct daNpc_Nz_c : fopNpc_npc_l {
    /* 0x7DC */ cXyz mShopOrigin;
    /* 0x7E8 */ be<s32> mCurMode;
    /* 0x7EC */ be<s8> mPreviousAnimation;
    /* 0x7ED */ be<s8> mAnimation;
    /* 0x7EE */ be<s8> mAnimationState;
    u8 _7EF;
    /* 0x7F0 */ request_of_phase_process_class mPhase;
    /* 0x7F8 */ request_of_phase_process_class mModelPhase;
    /* 0x800 */ gptr<mDoExt_McaMorf> mRatMorf;
    /* 0x804 */ be<f32> mTargetSpeed;
    /* 0x808 */ dBgS_ObjAcch mRatAcch;
    /* 0x9CC */ dBgS_AcchCir mRatAcchCir;
    /* 0xA0C */ be<s32> mStaffId;
    /* 0xA10 */ be<u8> mTalkState;
    u8 _A11;
    /* 0xA12 */ be<s16> mEventId;
    /* 0xA14 */ be<u8> mVisible;
    u8 _A15;
    /* 0xA16 */ be<s16> mMessageDelay;
    /* 0xA18 */ be<u32> mMessage;
    /* 0xA1C */ be<s16> mStockSelection;
    /* 0xA1E */ be<s16> mAnimationLoops;
    /* 0xA20 */ be<u8> mItemKind;
    /* 0xA21 */ be<u8> mSelectedItemSlot;
    /* 0xA22 */ be<s16> mBaitTimer;
    /* 0xA24 */ gptr<fopAc_ac_c> mBait;
    /* 0xA28 */ be<f32> mBaitDistance;
    /* 0xA2C */ u8 mSmokeCallback[0x20];
    /* 0xA4C */ u8 mTailMaterial[0x188];
    /* 0xBD4 */ be<u32> mTailState;
    /* 0xBD8 */ cXyz mTailPositions[10];
    /* 0xC50 */ cXyz mTailVelocities[10];
    /* 0xCC8 */ cXyz mTailAnchors[2];
    /* 0xCE0 */ u8 _CE0[0xC];
    /* 0xCEC */ be<u32> mShopItemId[2];
    /* 0xCF4 */ be<s16> mPurchaseCount;
    u8 _CF6[2];
};
WWHD_OFFSET(daNpc_Nz_c, mRatAcch, 0x808);
WWHD_OFFSET(daNpc_Nz_c, mRatAcchCir, 0x9CC);
WWHD_OFFSET(daNpc_Nz_c, mStaffId, 0xA0C);
WWHD_OFFSET(daNpc_Nz_c, mTailPositions, 0xBD8);
WWHD_OFFSET(daNpc_Nz_c, mShopItemId, 0xCEC);
WWHD_SIZE(daNpc_Nz_c, 0xCF8);

struct daNpc_Nz_HIO_c {
    be<s8> mNo;
    u8 _01;
    be<s16> mHeadMinY, mHeadMaxY, mHeadMinX, mHeadMaxX;
    be<s16> mBackboneMinY, mBackboneMaxY, mBackboneMinX, mBackboneMaxX;
    be<s16> mLookVelocity, mTalkingLookVelocity;
    be<u8> mDebug;
    u8 _17;
    be<f32> mRange, mBaitRange, mSearchRange;
    be<u32> mVtable;
};
WWHD_SIZE(daNpc_Nz_HIO_c, 0x28);
