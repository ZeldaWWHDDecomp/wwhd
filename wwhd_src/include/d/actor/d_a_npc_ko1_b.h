// Joel and Zill creation, movement and drawing: private views of the common actor.
// HD: GameCube leaves this actor layout unknown; offsets are measured from verified HD code.
#pragma once
#include "d/actor/d_a_npc_ko1.h"
#include "gc_api/f_op/f_op_actor.h"
struct Ko1MovementFields {
    u8 _0[0x7E4];
    /* 0x7E4 */ be<s8> m_hed_jnt_num;  /* was mBodyJoint (misnamed): view of daNpc_Ko1_c m_hed_jnt_num (d_a_npc_ko1.h); one name per field. create_Anm 022736FC asserts it with the original text "m_hed_jnt_num >= 0"; it is the "head" joint (setMtx puts the head model on it), not a body joint */
    u8 _7E5[0x1];
    /* 0x7E6 */ be<s8> m_armR2_jnt_num;  /* was mItemJoint (misnamed): view of daNpc_Ko1_c m_armR2_jnt_num (d_a_npc_ko1.h); one name per field. create_Anm 022736FC asserts it with the original text "m_armR2_jnt_num >= 0" (the "armR2" joint the item model is put on) */
    u8 _7E7[0x35];
    /* 0x81C */ gptr<mDoExt_McaMorf> mpBalloonMorf;
    /* 0x820 */ gptr<J3DModel> mpItemModel;
    /* 0x824 */ gptr<mDoExt_McaMorf> mpHeadMorf;
    u8 _828[0xFC];
    /* 0x924 */ be<u32> mPartnerId;
    /* 0x928 */ be<u32> mOtherPartnerId;
    /* 0x92C */ be<u8> mPartnerCount;
    u8 _92D[0x19];
    /* 0x946 */ csXyz mModelAngle;
    u8 _94C[0xC];
    /* 0x958 */ cXyz mLookPos;  /* was mLookTarget (misnamed): view of daNpc_Ko1_c mLookPos (d_a_npc_ko1.h); one name per field. lookBack 022758B8 looks at it in look state 2, as daNpc_Bm1_c::mLookPos (tww Bm1 dstPos = m82C); setStt 02278134 sets the partner's eye position; the class / _d1.h name stays */
    /* 0x964 */ cXyz mTargetPos;  /* was mMoveTarget (misnamed): view of daNpc_Ko1_c mTargetPos (d_a_npc_ko1.h); one name per field. setStt 02278134 sets the path point or the player's position and the move functions head for it, as daNpc_Bm1_c::mTargetPos; the class / _d1.h name stays */
    u8 _970[0xC];
    /* 0x97C */ cXyz mAreaCenter;  /* was mSpawnPos (misnamed): view of daNpc_Ko1_c mAreaCenter (d_a_npc_ko1.h); one name per field. createInit 02274E28 sets current.pos, but most uses (wait_4 .. walk_1, chk_areaIn) take it as the centre of the player-check area; the class / _d1.h name stays */
    /* 0x988 */ cXyz mGroundNormal;
    /* 0x994 */ be<f32> mPrevMorfFrame;  /* was mPreviousFrame (misnamed): view of daNpc_Ko1_c mPrevMorfFrame (d_a_npc_ko1.h); one name per field. setMtx 02274A7C keeps the last morf frame to see the animation wrap; same code as daNpc_Km1_c / daNpc_Ls1_c::mPrevMorfFrame (after tww Ls1) */
    u8 _998[0x4];
    /* 0x99C */ be<f32> mMoveSpeed;  /* was mTargetSpeed (misnamed): view of daNpc_Ko1_c mMoveSpeed (d_a_npc_ko1.h); one name per field. setStt 02278134 sets the HIO walk / run speed that ko_clcMovSpd chases; the class / _c.h / _d1.h name stays */
    /* 0x9A0 */ be<f32> mVerticalSpeed;  /* was mSwimSpeed (misnamed): view of daNpc_Ko1_c mVerticalSpeed (d_a_npc_ko1.h); one name per field. setStt / swim_1 / swim_2 set the speed.y target for every swim step, not only one swim speed; the class / _d1.h name stays */
    /* 0x9A4 */ be<f32> mMoveAcceleration;  /* was mAcceleration (misnamed): view of daNpc_Ko1_c mMoveAcceleration (d_a_npc_ko1.h); one name per field. the class / _c.h / _d1.h name stays (same field: setStt 02278134 sets it, ko_clcMovSpd steps speedF by it) */
    u8 _9A8[0x4];
    /* 0x9AC */ be<f32> mPassDist;  /* was mArrivalRadius (misnamed): view of daNpc_Ko1_c mPassDist (d_a_npc_ko1.h); one name per field. ko_movPass 022761AC: the point counts as reached once the XZ distance is <= it (setStt sets the HIO value); same code as daNpc_Bm1_c::mPassDist (tww Bm1 m858); it is no turn rate */
    /* 0x9B0 */ be<s16> mJointHeadY;  /* was mPreviousHeadY (misnamed): view of daNpc_Ko1_c mJointHeadY (d_a_npc_ko1.h); one name per field. lookBack 022758B8 stores the joint control's head Y angle and chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointHeadY (after tww Ls1) */
    /* 0x9B2 */ be<s16> mJointBackboneY;  /* was mPreviousBackboneY (misnamed): view of daNpc_Ko1_c mJointBackboneY (d_a_npc_ko1.h); one name per field. lookBack 022758B8 stores the joint control's backbone Y angle and chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointBackboneY (after tww Ls1) */
    /* 0x9B4 */ be<s16> mActorAngleY;  /* was mPreviousBodyY (misnamed): view of daNpc_Ko1_c mActorAngleY (d_a_npc_ko1.h); one name per field. lookBack 022758B8 stores current.angle.y and chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mActorAngleY (after tww Ls1); the class / _c.h name stays */
    u8 _9B6[0xA];
    /* 0x9C0 */ be<s16> mZigzagTimer;
    u8 _9C2[0x4];
    /* 0x9C6 */ be<s16> mTurnVelocity;
    /* 0x9C8 */ be<s16> mDesiredYRot;  /* was mLookAngle (misnamed): view of daNpc_Ko1_c mDesiredYRot (d_a_npc_ko1.h); one name per field. lookBack 022758B8 uses it as the look-at's Y angle in look state 3 and setStt / the waits / manzai set mInitialAngle.y; same code as daNpc_Bm1_c::mDesiredYRot (tww Bm1 desiredYrot = m876) */
    u8 _9CA[0x6];
    /* 0x9D0 */ be<u8> mAnimationEnded;
    u8 _9D1[0x3];
    /* 0x9D4 */ be<u8> mZigzagDirection;
    /* 0x9D5 */ be<u8> mSleepState;  /* was mBalloonState (misnamed): view of daNpc_Ko1_c mSleepState (d_a_npc_ko1.h); one name per field. neru_1 0227A110 / neru_2 0227A1C8 step it; setMtx only reads it to sync the sleep balloon; the class / _d1.h / _d2.h name stays */
    u8 _9D6[0x1];
    /* 0x9D7 */ be<u8> mHidden;
    /* 0x9D8 */ be<u8> mKeepShape;
    /* 0x9D9 */ be<u8> mSkipMovement;
    /* 0x9DA */ be<u8> mDrawHidden;
    /* 0x9DB */ be<u8> mbMoveFinished;  /* was mPointReached (misnamed): view of daNpc_Ko1_c mbMoveFinished (d_a_npc_ko1.h); one name per field. ko_nMove 022763A8 also sets it when a jump lands, and the walk / down states wait for it; the class / _d1.h name stays */
    /* 0x9DC */ be<u8> mPreviousGroundHit;
    u8 _9DD[0x8];
    /* 0x9E5 */ be<u8> mTalking;  /* was mTalkRequested (misnamed): view of daNpc_Ko1_c mTalking (d_a_npc_ko1.h); one name per field. checkOrder 02275504 sets it on the talk command, the waits start the talk on it, talk_1 clears it; same code as daNpc_Bm1_c::mTalking (tww Bm1 m895) */
    /* 0x9E6 */ be<u8> mHeadOnlyFollow;  /* was mHeadOnly (misnamed): view of daNpc_Ko1_c mHeadOnlyFollow (d_a_npc_ko1.h); one name per field. lookBack 022758B8 passes it as the joint look-at's head-only flag (setStt sets 1 in the moving states); same code as daNpc_Bm1_c::mHeadOnlyFollow (tww Bm1 headOnlyFollow = m896) */
    /* 0x9E7 */ be<u8> mInDemo;
    u8 _9E8[0x20];
    /* 0xA08 */ be<u8> mMoveMode;
    /* 0xA09 */ be<u8> mPreviousMoveMode;
    /* 0xA0A */ be<u8> mCutIndex;
    /* 0xA0B */ be<u8> mCutAction;
    /* 0xA0C */ be<u8> mAnmAtrNo;  /* was mAnimationAttribute (misnamed): view of daNpc_Ko1_c mAnmAtrNo (d_a_npc_ko1.h); one name per field. chg_anmAtr 02276EB8 stores the message animation attribute and setAnm_ATR picks the animation by it; same code as daNpc_Bm1_c::mAnmAtrNo (tww Bm1 m8F7) */
    /* 0xA0D */ be<u8> mMesgAnimeTag;  /* was mAnimationTag (misnamed): view of daNpc_Ko1_c mMesgAnimeTag (d_a_npc_ko1.h); one name per field. anmAtr 02276F58 stores the message's animation tag and chg_anmTag / control_anmTag act on it; same code as daNpc_Bm1_c::mMesgAnimeTag (tww Bm1 m8F8) */
    u8 _A0E[0x1];
    /* 0xA0F */ be<u8> mAnmNum;  /* was mAnimationNo (misnamed): view of daNpc_Ko1_c mAnmNum (d_a_npc_ko1.h); one name per field. setAnm_anm 02276B9C stores the animation number; same code as daNpc_Bm1_c::mAnmNum (tww Bm1 mAnmNum) */
    u8 _A10[0x2];
    /* 0xA12 */ be<s8> mOrderEvent;  /* was mEventState (misnamed): view of daNpc_Ko1_c mOrderEvent (d_a_npc_ko1.h); one name per field. eventOrder 02276578 orders the talk by it and checkOrder clears it, the waits set 2; same code as daNpc_Bm1_c::mOrderEvent (tww Bm1 m8FD); it is no look mode */
    /* 0xA13 */ be<s8> mStatus;  /* was mActionState (misnamed): view of daNpc_Ko1_c mStatus (d_a_npc_ko1.h); one name per field. setStt 02278134 stores the state, setAnm and the action functions switch on it; the class / _c.h / _d1.h / _d2.h name stays (daNpc_Bm1_c::mStatus) */
    u8 _A14[0x1];
    /* 0xA15 */ be<s8> mLookBackState;  /* was mLookMode (misnamed): view of daNpc_Ko1_c mLookBackState (d_a_npc_ko1.h); one name per field. lookBack 022758B8 switches on it (1 player, 2 mLookPos, 3 mDesiredYRot) and chg_anmAtr sets 1 with m_jnt.mbTrn; same code as daNpc_Bm1_c::mLookBackState (tww Bm1) */
    u8 _A16[0x2];
    /* 0xA18 */ be<s8> mActionStatus;  /* was mActionStep (misnamed): view of daNpc_Ko1_c mActionStatus (d_a_npc_ko1.h); one name per field. set_action 022742C0 sets 9 / 0 and the action functions step it; same code as daNpc_Bm1_c::mActionStatus (after tww daNpc_Bs1_c mActionStatus) */
    u8 _A19[0x3];
};
WWHD_SIZE(Ko1MovementFields, 0xA1C);
WWHD_OFFSET(Ko1MovementFields, m_hed_jnt_num, 0x7E4);
WWHD_OFFSET(Ko1MovementFields, m_armR2_jnt_num, 0x7E6);
WWHD_OFFSET(Ko1MovementFields, mPartnerId, 0x924);
WWHD_OFFSET(Ko1MovementFields, mOtherPartnerId, 0x928);
WWHD_OFFSET(Ko1MovementFields, mPartnerCount, 0x92C);
WWHD_OFFSET(Ko1MovementFields, mModelAngle, 0x946);
WWHD_OFFSET(Ko1MovementFields, mLookPos, 0x958);
WWHD_OFFSET(Ko1MovementFields, mTargetPos, 0x964);
WWHD_OFFSET(Ko1MovementFields, mAreaCenter, 0x97C);
WWHD_OFFSET(Ko1MovementFields, mGroundNormal, 0x988);
WWHD_OFFSET(Ko1MovementFields, mPrevMorfFrame, 0x994);
WWHD_OFFSET(Ko1MovementFields, mMoveSpeed, 0x99C);
WWHD_OFFSET(Ko1MovementFields, mVerticalSpeed, 0x9A0);
WWHD_OFFSET(Ko1MovementFields, mMoveAcceleration, 0x9A4);
WWHD_OFFSET(Ko1MovementFields, mPassDist, 0x9AC);
WWHD_OFFSET(Ko1MovementFields, mJointHeadY, 0x9B0);
WWHD_OFFSET(Ko1MovementFields, mJointBackboneY, 0x9B2);
WWHD_OFFSET(Ko1MovementFields, mActorAngleY, 0x9B4);
WWHD_OFFSET(Ko1MovementFields, mZigzagTimer, 0x9C0);
WWHD_OFFSET(Ko1MovementFields, mTurnVelocity, 0x9C6);
WWHD_OFFSET(Ko1MovementFields, mDesiredYRot, 0x9C8);
WWHD_OFFSET(Ko1MovementFields, mAnimationEnded, 0x9D0);
WWHD_OFFSET(Ko1MovementFields, mZigzagDirection, 0x9D4);
WWHD_OFFSET(Ko1MovementFields, mSleepState, 0x9D5);
WWHD_OFFSET(Ko1MovementFields, mHidden, 0x9D7);
WWHD_OFFSET(Ko1MovementFields, mKeepShape, 0x9D8);
WWHD_OFFSET(Ko1MovementFields, mSkipMovement, 0x9D9);
WWHD_OFFSET(Ko1MovementFields, mDrawHidden, 0x9DA);
WWHD_OFFSET(Ko1MovementFields, mbMoveFinished, 0x9DB);
WWHD_OFFSET(Ko1MovementFields, mPreviousGroundHit, 0x9DC);
WWHD_OFFSET(Ko1MovementFields, mTalking, 0x9E5);
WWHD_OFFSET(Ko1MovementFields, mHeadOnlyFollow, 0x9E6);
WWHD_OFFSET(Ko1MovementFields, mInDemo, 0x9E7);
WWHD_OFFSET(Ko1MovementFields, mMoveMode, 0xA08);
WWHD_OFFSET(Ko1MovementFields, mPreviousMoveMode, 0xA09);
WWHD_OFFSET(Ko1MovementFields, mCutIndex, 0xA0A);
WWHD_OFFSET(Ko1MovementFields, mCutAction, 0xA0B);
WWHD_OFFSET(Ko1MovementFields, mAnmAtrNo, 0xA0C);
WWHD_OFFSET(Ko1MovementFields, mMesgAnimeTag, 0xA0D);
WWHD_OFFSET(Ko1MovementFields, mAnmNum, 0xA0F);
WWHD_OFFSET(Ko1MovementFields, mOrderEvent, 0xA12);
WWHD_OFFSET(Ko1MovementFields, mStatus, 0xA13);
WWHD_OFFSET(Ko1MovementFields, mLookBackState, 0xA15);
WWHD_OFFSET(Ko1MovementFields, mActionStatus, 0xA18);
template<class T, class U> inline T* Ko1B_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline Ko1MovementFields& Ko1B_fields(daNpc_Ko1_c* p) { return *Ko1B_view<Ko1MovementFields>(p); }
struct Ko1MovementParameters {
    u8 _00[4];
    be<s16> mHeadMinX, mHeadMaxX, mHeadMinY, mHeadMaxY;
    be<s16> mBackboneMinX, mBackboneMaxX, mBackboneMinY, mBackboneMaxY;
    be<s16> mJointScale, mTurnMaxVelocity;
    u8 _18[4]; be<u8> mDebugColors; u8 _1D[3];
    be<s16> mAngleStep; u8 _22[2];
    be<f32> mWalkAnimationRate, mWalkMaxSpeed; u8 _2C[8];
    be<f32> mRunAnimationRate, mRunMaxSpeed; u8 _3C[0x24];
};
WWHD_SIZE(Ko1MovementParameters, 0x60);
WWHD_OFFSET(Ko1MovementParameters, mHeadMinX, 0x4);
WWHD_OFFSET(Ko1MovementParameters, mHeadMaxX, 0x6);
WWHD_OFFSET(Ko1MovementParameters, mHeadMinY, 0x8);
WWHD_OFFSET(Ko1MovementParameters, mHeadMaxY, 0xA);
WWHD_OFFSET(Ko1MovementParameters, mBackboneMinX, 0xC);
WWHD_OFFSET(Ko1MovementParameters, mBackboneMaxX, 0xE);
WWHD_OFFSET(Ko1MovementParameters, mBackboneMinY, 0x10);
WWHD_OFFSET(Ko1MovementParameters, mBackboneMaxY, 0x12);
WWHD_OFFSET(Ko1MovementParameters, mJointScale, 0x14);
WWHD_OFFSET(Ko1MovementParameters, mTurnMaxVelocity, 0x16);
WWHD_OFFSET(Ko1MovementParameters, mDebugColors, 0x1C);
WWHD_OFFSET(Ko1MovementParameters, mAngleStep, 0x20);
WWHD_OFFSET(Ko1MovementParameters, mWalkAnimationRate, 0x24);
WWHD_OFFSET(Ko1MovementParameters, mWalkMaxSpeed, 0x28);
WWHD_OFFSET(Ko1MovementParameters, mRunAnimationRate, 0x34);
WWHD_OFFSET(Ko1MovementParameters, mRunMaxSpeed, 0x38);
struct Ko1PathFlags { u8 _00[5]; be<u8> mClosed; };
WWHD_OFFSET(Ko1PathFlags, mClosed, 5);
struct Ko1ModelAnimations { u8 _00[0x38]; be<u32> mpBtpAnimation; };
WWHD_OFFSET(Ko1ModelAnimations, mpBtpAnimation, 0x38);
void Ko1B_outprod(const cXyz*, cXyz*, const cXyz*);
void Ko1B_baseGroundDelete(void*, s32);
BOOL Ko1B_play(mDoExt_McaMorf*, cXyz*, u32, s32);
BOOL Ko1B_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8);
BOOL Ko1B_setPath(dNpc_PathRun_l*, u8, s8, u8);
BOOL Ko1B_checkPoint(dNpc_PathRun_l*, cXyz*, u32);
BOOL Ko1B_nextPoint(dNpc_PathRun_l*);
void Ko1B_constructNpc(daNpc_Ko1_c*);
void Ko1B_constructBtp(void*);
void Ko1B_constructEvent(dNpc_EventCut_c*);
void Ko1B_constructRipple(dPa_rippleEcallBack_l*);
u32 Ko1B_charDecide(daNpc_Ko1_c*, u32);
BOOL Ko1B_checkCreateHeap(fopAc_ac_c*);
void* Ko1B_searchHna(void*, void*);
void* Ko1B_searchBou(void*, void*);
void* Ko1B_searchOb(void*, void*);
void* Ko1B_demoActor(u32, u8);
J3DAnmTexPattern* Ko1B_demoPattern(void*, const char*);
// HD: preserve the full reverb/result registers instead of extending the GameCube narrow type.
inline void Ko1B_initializeColor(be<u32>& guard, be<u32>& color, const be<u32>& initial) {
    if (guard == 0) { guard = 1; memcpy_g(&color, &initial, 4); }
}

// HD: these callbacks are defined by Part A; register their guest addresses without duplicating them.
inline gabi::HostFn Ko1B_checkCreateHeapAddress((const void*)&Ko1B_checkCreateHeap, 0x0227424C);
inline gabi::HostFn Ko1B_searchHnaAddress((const void*)&Ko1B_searchHna, 0x02273170);
inline gabi::HostFn Ko1B_searchBouAddress((const void*)&Ko1B_searchBou, 0x022731F0);
inline gabi::HostFn Ko1B_searchObAddress((const void*)&Ko1B_searchOb, 0x02273270);
inline const char* Ko1B_staffName(u32 address) { return gabi::at<const char>(address); }

WWHD_OFFSET(Ko1MovementFields, mpBalloonMorf, 0x81C);
WWHD_OFFSET(Ko1MovementFields, mpItemModel, 0x820);
WWHD_OFFSET(Ko1MovementFields, mpHeadMorf, 0x824);
enum Ko1MoveMode { KO1_MOVE_WAIT, KO1_MOVE_WALK, KO1_MOVE_RUN, KO1_MOVE_SWIM, KO1_MOVE_JUMP };
enum Ko1MoveResult { KO1_MOVING, KO1_POINT_REACHED, KO1_PATH_END };
enum Ko1LookMode { KO1_LOOK_FORWARD, KO1_LOOK_PLAYER, KO1_LOOK_TARGET, KO1_LOOK_ANGLE };
enum Ko1EventState { KO1_EVENT_NONE, KO1_EVENT_SPEAK, KO1_EVENT_TALK };
enum Ko1CharacterType { KO1_JOEL, KO1_ZILL };
enum Ko1SpecificType { KO1_HNA_0, KO1_HNA_1, KO1_HNA_2, KO1_HNA_3, KO1_HNA_4, KO1_BOU_0, KO1_BOU_1, KO1_BOU_2, KO1_BOU_3 };
