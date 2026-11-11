#pragma once
#include "d/actor/d_a_npc_kg1.h"
#include "gc_api/tu.h"

// HD: partial views of the verified Salvatore/minigame layout.
struct Kg1Actor : fopNpc_npc_l {
    gptr<J3DModel> mpPropModel;
    /* mBtpAnm (0x7E0): the prop texture pattern animation, kg1_core::daNpc_Kg1_c::mBtpAnm (tww mBtpAnm at GameCube 0x6C8); its mFrameCtrl.mFrame (0x7E4) was mTalkAnimationWeight (misnamed): daNpc_Kg1_setAnm 02265E24 sets the frame to 1.0 when animation 9's morf reaches frame 31, daNpc_Kg1_wait_action 02266D60 sets 0.0 when the talk starts; daNpc_Kg1Draw 02266B24 enters the prop pattern at this frame */
    mDoExt_btpAnm mBtpAnm;
    u8 mPadding854[0x95A - 0x854];
    /* mTalking (0x95A) was mTalkActive (misnamed): kg1_core::daNpc_Kg1_c::mTalking; one name per field */
    /* mOrderEvent (0x95C) was mEventOrder (misnamed): kg1_core::daNpc_Kg1_c::mOrderEvent; one name per field */
    be<u8> mTalking, mbAttention, mOrderEvent;
    u8 mPadding95D[0x978 - 0x95D];
    be<s8> mAnimation, mRequestedAnimation, mPreviousAnimation;
    be<u8> mSpecialAnimation, mResponseComplete, mTalkEnabled;
    u8 mPadding97E[0x99E - 0x97E];
    be<u8> mSequenceFlags[6];
    /* mbForceNextStatus (0x9A5) was m779: kg1_core::daNpc_Kg1_c::mbForceNextStatus */
    be<u8> m778, mbForceNextStatus, mGameBoardScore, mEndGameTimer;
    be<u32> mPresentActor;
    be<s32> mWaitMode;
    be<s16> mEventIds[3];
    u8 mTail[2];
};
WWHD_OFFSET(Kg1Actor, mBtpAnm, 0x7E0);
WWHD_OFFSET(Kg1Actor, mbAttention, 0x95B);
WWHD_OFFSET(Kg1Actor, mAnimation, 0x978);
WWHD_OFFSET(Kg1Actor, mSequenceFlags, 0x99E);
WWHD_OFFSET(Kg1Actor, mEndGameTimer, 0x9A7);
WWHD_OFFSET(Kg1Actor, mWaitMode, 0x9AC);
WWHD_SIZE(Kg1Actor, 0x9B8);
struct Kg1Morf {
    u8 mPrefix[0x90];
    gptr<J3DModel> mpModel;
    u8 mPadding94[4];
    be<f32> mRate, mFrame;
    u8 mPaddingA0[2];
    be<s16> mEnd;
    u8 mPaddingA4[3];
    be<u8> mFlags;
};
WWHD_OFFSET(Kg1Morf, mFrame, 0x9C);
WWHD_OFFSET(Kg1Morf, mFlags, 0xA7);
// GameCube dLib_anm_prm_c field names; HD retains the 16-byte layout.
struct Kg1AnimationParameter {
    be<s8> mAnmIdx, mNextPrmIdx;
    be<s16> field_0x02;
    be<f32> mMorf, mPlaySpeed;
    be<s32> mLoopMode;
};
WWHD_SIZE(Kg1AnimationParameter, 0x10);
struct Kg1AnimationWords {
    be<u32> words[48];
};
struct Kg1LocalAnimationWords {
    u8 callFrame[32];
    be<u32> words[48];
};
struct Kg1ResourceIndices {
    be<s32> values[10];
};
struct Kg1TextureIndices {
    be<s16> values[12];
};
struct Kg1ResourceControl {
    gptr<void> control;
};
struct Kg1ResourceName {
    u8 callFrame[8];
    SafeString name;
};
BOOL daNpc_Kg1_initTexPatternAnm(daNpc_Kg1_c *, s32, s32);
J3DAnmTransform *Kg1_getResource(void *, SafeString *, s32);
void Kg1_setMorfAnm(Kg1Morf *, J3DAnmTransform *, s32, f32, f32, f32, f32, u32);
