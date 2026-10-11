// Mila message and action part: private views of the shared actor layout.
// HD: measured names for fields absent from the nonmatching GameCube header.
// Views add no actor storage and leave the other Mila parts unchanged.
#pragma once
#include "d/actor/d_a_npc_kk1.h"
struct Kk1ActionFields {
    u8 _0[0x7D6];
    /* 0x7D6 */ be<u16> mPartnerEventStatus;
    u8 _7D8[0x94];
    /* 0x86C */ be<u32> mPartnerId;
    u8 _870[0x18];
    /* 0x888 */ csXyz mHomeAngle;
    u8 _88E[0x1A];
    /* 0x8A8 */ cXyz mLookTarget;
    u8 _8B4[0x2C];
    /* 0x8E0 */ csXyz mPreviousLookAngles;
    u8 _8E6[0x2];
    /* 0x8E8 */ be<u32> mActionResult;
    u8 _8EC[0x4];
    /* 0x8F0 */ be<s16> mPickEventId;
    u8 _8F2[0xA];
    /* 0x8FC */ be<s16> mEventAction;
    u8 _8FE[0x4];
    /* 0x902 */ be<s16> mTurnRatio;
    /* 0x904 */ be<s16> mTurnMaxStep;
    u8 _906[0x2];
    /* 0x908 */ be<s16> mTurnTimer;
    u8 _90A[0x4];
    /* 0x90E */ be<s16> mAfterTalkTimer;
    u8 _910[0x2];
    /* 0x912 */ be<s16> mTalkLookTimer;
    /* 0x914 */ be<s16> mActionTimer;
    u8 _916[0x4];
    /* 0x91A */ be<s16> mTurnStartAngle;
    u8 _91C[0x2];
    /* 0x91E */ be<s16> mRefusedAnswerCount;
    u8 _920[0x2];
    /* 0x922 */ be<u8> mAnimationEnded;
    u8 _923[0x1];
    /* 0x924 */ be<u8> mPresentedItem;
    /* 0x925 */ be<u8> mSwitchNo;
    /* 0x926 */ be<u8> mOnPath;
    /* 0x927 */ be<u8> mWaitingAtPathEnd;
    /* 0x928 */ be<u8> mPartnerEventEnabled;
    u8 _929[0x1];
    /* 0x92A */ be<u8> mNightMessageFlag;
    u8 _92B[0x7];
    /* 0x932 */ be<u8> mAttentionSelected;
    /* 0x933 */ be<u8> mTalkRequested;
    /* 0x934 */ be<u8> mUseLookTarget;
    /* 0x935 */ be<u8> mPlayerFar;
    u8 _936[0x18C];
    /* 0xAC2 */ be<u8> mCommentAction;
    /* 0xAC3 */ be<u8> mTalkAnimation;
    /* 0xAC4 */ be<u8> mMessageTag;
    u8 _AC5[0x1];
    /* 0xAC6 */ be<u8> mAnimationNo;
    /* 0xAC7 */ be<u8> mEventState;
    /* 0xAC8 */ be<u8> mActionState;
    /* 0xAC9 */ be<u8> mPreviousState;
    /* 0xACA */ be<u8> mLookMode;
    u8 _ACB[0x1];
    /* 0xACC */ be<s8> mMessageType;
    /* 0xACD */ be<u8> mActionStep;
    /* 0xACE */ be<u8> mMessageAnimationStarted;
    u8 _ACF[0x1];
};
WWHD_SIZE(Kk1ActionFields, 0xAD0);
WWHD_OFFSET(Kk1ActionFields, mPartnerId, 0x86C);
WWHD_OFFSET(Kk1ActionFields, mHomeAngle, 0x888);
WWHD_OFFSET(Kk1ActionFields, mLookTarget, 0x8A8);
WWHD_OFFSET(Kk1ActionFields, mPreviousLookAngles, 0x8E0);
WWHD_OFFSET(Kk1ActionFields, mActionResult, 0x8E8);
WWHD_OFFSET(Kk1ActionFields, mPickEventId, 0x8F0);
WWHD_OFFSET(Kk1ActionFields, mEventAction, 0x8FC);
WWHD_OFFSET(Kk1ActionFields, mTurnRatio, 0x902);
WWHD_OFFSET(Kk1ActionFields, mTurnMaxStep, 0x904);
WWHD_OFFSET(Kk1ActionFields, mTurnTimer, 0x908);
WWHD_OFFSET(Kk1ActionFields, mAfterTalkTimer, 0x90E);
WWHD_OFFSET(Kk1ActionFields, mTalkLookTimer, 0x912);
WWHD_OFFSET(Kk1ActionFields, mActionTimer, 0x914);
WWHD_OFFSET(Kk1ActionFields, mTurnStartAngle, 0x91A);
WWHD_OFFSET(Kk1ActionFields, mRefusedAnswerCount, 0x91E);
WWHD_OFFSET(Kk1ActionFields, mAnimationEnded, 0x922);
WWHD_OFFSET(Kk1ActionFields, mPresentedItem, 0x924);
WWHD_OFFSET(Kk1ActionFields, mSwitchNo, 0x925);
WWHD_OFFSET(Kk1ActionFields, mOnPath, 0x926);
WWHD_OFFSET(Kk1ActionFields, mWaitingAtPathEnd, 0x927);
WWHD_OFFSET(Kk1ActionFields, mPartnerEventEnabled, 0x928);
WWHD_OFFSET(Kk1ActionFields, mNightMessageFlag, 0x92A);
WWHD_OFFSET(Kk1ActionFields, mAttentionSelected, 0x932);
WWHD_OFFSET(Kk1ActionFields, mTalkRequested, 0x933);
WWHD_OFFSET(Kk1ActionFields, mUseLookTarget, 0x934);
WWHD_OFFSET(Kk1ActionFields, mPlayerFar, 0x935);
WWHD_OFFSET(Kk1ActionFields, mCommentAction, 0xAC2);
WWHD_OFFSET(Kk1ActionFields, mTalkAnimation, 0xAC3);
WWHD_OFFSET(Kk1ActionFields, mMessageTag, 0xAC4);
WWHD_OFFSET(Kk1ActionFields, mAnimationNo, 0xAC6);
WWHD_OFFSET(Kk1ActionFields, mEventState, 0xAC7);
WWHD_OFFSET(Kk1ActionFields, mActionState, 0xAC8);
WWHD_OFFSET(Kk1ActionFields, mPreviousState, 0xAC9);
WWHD_OFFSET(Kk1ActionFields, mLookMode, 0xACA);
WWHD_OFFSET(Kk1ActionFields, mMessageType, 0xACC);
WWHD_OFFSET(Kk1ActionFields, mActionStep, 0xACD);
WWHD_OFFSET(Kk1ActionFields, mMessageAnimationStarted, 0xACE);
template<class T, class U> inline T* Kk1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline Kk1ActionFields& Kk1_fields(daNpc_Kk1_c* actor) { return *Kk1_view<Kk1ActionFields>(actor); }
struct Kk1ActionParameters {
    u8 _00[0x20]; be<f32> mAttentionYOffset;
    u8 _24[2]; be<s16> mPickTimer, mTurnMaxStep, mTurnRatio, mTurnTimer, mTalkLookTimer;
    be<s16> mAngleScale, mAngleMaxStep;
    be<f32> mPathPointRadius, mWalkSpeed, mWalkAcceleration, mAnimationSpeedScale;
    u8 _44[0x18]; be<f32> mDetectionRadius;
};
WWHD_SIZE(Kk1ActionParameters, 0x60);
WWHD_OFFSET(Kk1ActionParameters, mPickTimer, 0x26);
WWHD_OFFSET(Kk1ActionParameters, mAngleScale, 0x30);
WWHD_OFFSET(Kk1ActionParameters, mDetectionRadius, 0x5C);
struct Kk1Tama : fopAc_ac_c {
    be<u32> mOwnerId; u8 _3B0[0x71C-0x3B0]; be<f32> mSpeed;
};
WWHD_OFFSET(Kk1Tama, mOwnerId, 0x3AC);
WWHD_OFFSET(Kk1Tama, mSpeed, 0x71C);
struct Kk1PlayerFlags { u8 _00[0x600]; be<u32> mFlags; };
WWHD_OFFSET(Kk1PlayerFlags, mFlags, 0x600);
struct Kk1CoHitObject { u8 _00[0x44]; gptr<dCcD_Stts> mpStts; };
WWHD_OFFSET(Kk1CoHitObject, mpStts, 0x44);
inline Kk1CoHitObject* Kk1_hitObject(u32 address) { return gabi::at<Kk1CoHitObject>(address); }
s16 Kk1_targetAngleX(const cXyz*, const cXyz*);
fopAc_ac_c* Kk1_fastCreate(s16, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32, void*);
u32 Kk1_coHitObject(dCcD_GObjInf*);
f32 Kk1_squareDistance(const cXyz*, const cXyz*);
void Kk1_soundStart(u32, cXyz*);
void Kk1_pathDecIdx(dNpc_PathRun_l*);
u8 Kk1_pathNextIdx(dNpc_PathRun_l*);
u8 Kk1_pathMaxPoint(dNpc_PathRun_l*);
s32 Kk1_shopCreateCount();
void Kk1_pathPoint(cXyz*, dNpc_PathRun_l*);
struct Kk1MessageSelection { u8 _00[0x948]; be<u32> mSelectedAnswer; };
WWHD_OFFSET(Kk1MessageSelection, mSelectedAnswer, 0x948);
inline u32 Kk1_selectedAnswer() { return Kk1_view<Kk1MessageSelection>(fopNpc_npc_c::mpCurrMsg.get())->mSelectedAnswer; }

inline Kk1PlayerFlags* Kk1_playerFlags(u32 address) { return gabi::at<Kk1PlayerFlags>(address); }
inline dCcD_Stts* Kk1_status(u32 address) { return gabi::at<dCcD_Stts>(address); }
inline fopAc_ac_c* Kk1_actor(u32 address) { return gabi::at<fopAc_ac_c>(address); }

struct Kk1PathFlags { u8 _00[5]; be<u8> mClosed; };
WWHD_OFFSET(Kk1PathFlags, mClosed, 5);

WWHD_OFFSET(Kk1ActionFields, mPartnerEventStatus, 0x7D6);
enum Kk1ActionState {
    KK1_ACTION_WAIT = 1, KK1_ACTION_TALK = 2, KK1_ACTION_WALK = 3,
    KK1_ACTION_WAIT_PARTNER = 4, KK1_ACTION_COMMENT = 5,
    KK1_ACTION_WAIT_NEAR = 6, KK1_ACTION_WAIT_FAR = 7
};
enum Kk1MessageStatus {
    KK1_MSG_TYPING = 6, KK1_MSG_DISPLAYED = 14, KK1_MSG_CONTINUES = 15,
    KK1_MSG_ENDS = 16, KK1_MSG_DESTROYED = 19
};
