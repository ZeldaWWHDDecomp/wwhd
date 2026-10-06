/* WWHD Btsw2 actor layout. */
#pragma once
#include "bindings.h"
struct daNpc_Btsw2_c : fopNpc_npc_c {
    /* 0x7DC */ u8 mPhase[8];
    /* 0x7E4 */ gptr<J3DModel> mpBagModel;
    /* 0x7E8 */ gptr<J3DModel> mpFlyerModel;
    /* 0x7EC */ gptr<void> mpTexPattern;
    /* 0x7F0 */ u8 mBtpAnm[0x74];
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ be<s8> mHandLeftJoint;
    /* 0x866 */ be<s8> mHandRightJoint;
    u8 _867;
    /* 0x868 */ be<s16> mBlinkTimer;
    u8 _86A[0x878 - 0x86A];
    /* 0x878 */ cXyz mAttentionBase;
    u8 _884[0x888 - 0x884];
    /* 0x888 */ csXyz mLookRotation;
    u8 _88E[3];
    /* 0x891 */ be<u8> mHasAttention;
    /* 0x892 */ be<u8> mTalkAccepted;
    u8 _893[0x89C - 0x893];
    /* 0x89C */ be<s16> mActionThisAdjustment;
    /* 0x89E */ be<s16> mActionVtableIndex;
    /* 0x8A0 */ be<u32> mActionTarget;
    /* 0x8A4 */ be<s8> mPathNumber;
    /* 0x8A5 */ be<u8> mPathFlags;
    /* 0x8A6 */ be<u8> mPathAdvance;
    u8 _8A7;
    /* 0x8A8 */ gptr<void> mpPath;
    /* 0x8AC */ be<s16> mFinalPathPoint;
    /* 0x8AE */ be<s16> mPathTimer;
    /* 0x8B0 */ be<s16> mPathPoint;
    /* 0x8B2 */ be<s16> mWaitTimer;
    /* 0x8B4 */ be<s8> mPathState;
    /* 0x8B5 */ be<s8> mAnimation;
    /* 0x8B6 */ be<s8> mEventOrder;
    /* 0x8B7 */ be<s8> mMode;
    u8 _8B8[2];
    /* 0x8BA */ be<s8> mActionStatus;
    u8 _8BB[1];
};
WWHD_OFFSET(daNpc_Btsw2_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Btsw2_c, mAttentionBase, 0x878);
WWHD_OFFSET(daNpc_Btsw2_c, mActionTarget, 0x8A0);
WWHD_OFFSET(daNpc_Btsw2_c, mActionStatus, 0x8BA);
WWHD_SIZE(daNpc_Btsw2_c, 0x8BC);
