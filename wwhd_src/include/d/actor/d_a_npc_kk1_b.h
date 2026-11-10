/* Mila event-part views. The GC header leaves these members unnamed; HD roles and offsets
 * are recovered from the verified event implementation. Shared actor layout remains unchanged. */
#pragma once
#include "d/actor/d_a_npc_kk1.h"
#include "gc_api/d/actor/d_a_player.h"
#include "d/d_path.h"
#include "gc_api/f_op/f_op_actor.h"

struct Kk1EventState {
    u8 _reserved0[0x7D6];
    /* 0x7D6 */ be<u16> mFrameFlags; // HD: cleared after the action and before looking.
    u8 _beforePartner[0x94];
    /* 0x86C */ be<u32> mPartnerId;
    /* 0x870 */ be<u32> mLookActorId;
    u8 _reserved1[0x8];
    /* 0x87C */ cXyz mHomePos;
    /* 0x888 */ csXyz mHomeAngle;
    /* 0x88E */ csXyz mModelAngle;  /* was mLastAngle (misnamed): view of daNpc_Kk1_c mModelAngle (d_a_npc_kk1.h); one name per field. createInit 0226BFD0 sets it and setMtx 0226BDCC rotates the model by it; _execute only refreshes it from current.angle; same code as daNpc_Ko1_c::mModelAngle; the Kk1CreationFields name stays */
    u8 _reserved2[0x14];
    /* 0x8A8 */ cXyz mLookPos;
    u8 _reserved3[0x2C];
    /* 0x8E0 */ csXyz mPreviousLookAngles;  /* was mLookAngle (misnamed): view of daNpc_Kk1_c mPreviousLookAngles (d_a_npc_kk1.h); one name per field. lookBack 0226E604 stores the look angles and chk_parts_notMov 0226F6EC compares them with the current ones; the Kk1ActionFields name stays */
    u8 _reserved4[0x2];
    /* 0x8E8 */ be<u32> mbSetEyePos;  /* was mMessageHandle (misnamed): view of daNpc_Kk1_c mbSetEyePos (d_a_npc_kk1.h); one name per field. the state function's result (wait_action1 02271084) decides whether setAttention 0226BD78 copies the eye position to eyePos; same code as daNpc_Bm1_c / daNpc_Ko1_c::mbSetEyePos (tww Bm1 mbSetEyePos = wait_1()) */
    /* 0x8EC */ be<s16> mEventIds[8];
    /* 0x8FC */ be<s16> mEventIdx;  /* was mEventIndex (misnamed): view of daNpc_Kk1_c mEventIdx (d_a_npc_kk1.h); one name per field. eventOrder 0226E92C stores the ordered event's index and checkOrder / event_proc pick mEventIds by it; same code as daNpc_Bm1_c::mEventIdx (tww Bm1 mEventIdx) */
    u8 _reserved6[0x4];
    /* 0x902 */ be<s16> mLookDelay;
    /* 0x904 */ be<s16> mLookTimer;
    /* 0x906 */ be<s16> mLookPoint;
    /* 0x908 */ be<s16> mCutTimer;
    /* 0x90A */ be<s16> mTurnTimer;
    u8 _reserved7[0x10];
    /* 0x91C */ be<s16> mLookYaw;
    u8 _reserved8[0x2];
    /* 0x920 */ be<s16> mKeepMoving;
    /* 0x922 */ be<u8> mAnimationEnded;  /* was mAnmEnded (misnamed): view of daNpc_Kk1_c mAnimationEnded (d_a_npc_kk1.h); one name per field. the Kk1CreationFields / Kk1ActionFields name stays (same field: play_animation 0226BB5C stores the morf's play() result) */
    u8 _reserved9[0x3];
    /* 0x926 */ be<u8> mOnPath;  /* was mFollowPath (misnamed): view of daNpc_Kk1_c mOnPath (d_a_npc_kk1.h); one name per field. the Kk1CreationFields / Kk1ActionFields name stays (same field: set while the actor walks its path; event_move 0226D83C follows the path while it is set) */
    u8 _reserved10[0x3];
    /* 0x92A */ be<u8> mByeComplete;
    /* 0x92B */ be<u8> mHideBody;
    u8 _reserved11[0x1];
    /* 0x92D */ be<u8> mDemoSuspend;
    /* 0x92E */ be<u8> mEffectHidden;  /* was mHideEffect (misnamed): view of daNpc_Kk1_c mEffectHidden (d_a_npc_kk1.h); one name per field. setAnm_anm 0226C660 sets it by the animation and _draw 0226ECA8 skips the effect model while it is set; the Kk1CreationFields name stays */
    /* 0x92F */ be<u8> mPreserveShapeAngle;
    /* 0x930 */ be<u8> mHide;
    /* 0x931 */ be<u8> mHomeInitialized;
    u8 _reserved12[0x2];
    /* 0x934 */ be<u8> mHeadOnlyFollow;  /* was mHeadOnly (misnamed): view of daNpc_Kk1_c mHeadOnlyFollow (d_a_npc_kk1.h); one name per field. lookBack 0226E604 passes it as the joint look-at's head-only flag; same code as daNpc_Bm1_c / daNpc_Ko1_c::mHeadOnlyFollow (tww Bm1 headOnlyFollow = m896) */
    u8 _reserved13[0x1];
    /* 0x936 */ be<u8> mInDemo;  /* was mDisableCollision (misnamed): view of daNpc_Kk1_c mInDemo (d_a_npc_kk1.h); one name per field. demo 0226C8D8 sets and clears it and returns by it; _execute only skips the collision while it is set; same name as daNpc_Ko1_c::mInDemo; the Kk1CreationFields name stays */
    u8 _reserved14[0x175];
    /* 0xAAC */ csXyz mEffectFrame;  /* was mEffectAnmFrame (misnamed): view of daNpc_Kk1_c mEffectFrame (d_a_npc_kk1.h); one name per field. play_eff_anm 0226BA48 steps the effect frames and setMtx / _draw use them; the Kk1CreationFields name stays */
    u8 _reserved15[0x2];
    /* 0xAB4 */ gptr<J3DModel> mEffectModel;
    u8 _reserved16[0x8];
    /* 0xAC0 */ be<u8> mCutIndex;
    /* 0xAC1 */ be<u8> mMoveMode;  /* was mMoveAnm (misnamed): view of daNpc_Kk1_c mMoveMode (d_a_npc_kk1.h); one name per field. event_move 0226D83C walks at 2 and runs otherwise (the path point's argument 2 / 3), setStt 0226CB44 sets 2 for the comment state; it selects the move speed, not a look state or an animation */
    /* 0xAC2 */ be<u8> mCommentAction;  /* was mPathArg (misnamed): view of daNpc_Kk1_c mCommentAction (d_a_npc_kk1.h); one name per field. cmmt_1 02270AE0 switches on it and the move_CMT_* functions reset it; event_move 0226D83C only clears it; the Kk1CreationFields / Kk1ActionFields name stays */
    u8 _reserved17[0x3];
    /* 0xAC6 */ be<u8> mAnmNum;  /* was mAnmState (misnamed): view of daNpc_Kk1_c mAnmNum (d_a_npc_kk1.h); one name per field. setAnm_anm 0226C660 stores the animation number and play_animation switches on it; same code as daNpc_Bm1_c::mAnmNum (tww Bm1 mAnmNum) */
    /* 0xAC7 */ be<u8> mOrderEvent;  /* was mEventOrder (misnamed): view of daNpc_Kk1_c mOrderEvent (d_a_npc_kk1.h); one name per field. eventOrder 0226E92C orders by it and checkOrder 0226C7DC clears it, the waits set 2; same code as daNpc_Bm1_c / daNpc_Ko1_c::mOrderEvent (tww Bm1 m8FD) */
    u8 _reserved18[0x2];
    /* 0xACA */ be<u8> mLookMode;
    u8 _tail[0x5];
};
WWHD_SIZE(Kk1EventState, 0xAD0);
WWHD_OFFSET(Kk1EventState, mPartnerId, 0x86C);
WWHD_OFFSET(Kk1EventState, mLookActorId, 0x870);
WWHD_OFFSET(Kk1EventState, mHomePos, 0x87C);
WWHD_OFFSET(Kk1EventState, mHomeAngle, 0x888);
WWHD_OFFSET(Kk1EventState, mModelAngle, 0x88E);
WWHD_OFFSET(Kk1EventState, mLookPos, 0x8A8);
WWHD_OFFSET(Kk1EventState, mPreviousLookAngles, 0x8E0);
WWHD_OFFSET(Kk1EventState, mbSetEyePos, 0x8E8);
WWHD_OFFSET(Kk1EventState, mEventIdx, 0x8FC);
WWHD_OFFSET(Kk1EventState, mLookDelay, 0x902);
WWHD_OFFSET(Kk1EventState, mLookTimer, 0x904);
WWHD_OFFSET(Kk1EventState, mLookPoint, 0x906);
WWHD_OFFSET(Kk1EventState, mCutTimer, 0x908);
WWHD_OFFSET(Kk1EventState, mTurnTimer, 0x90A);
WWHD_OFFSET(Kk1EventState, mLookYaw, 0x91C);
WWHD_OFFSET(Kk1EventState, mKeepMoving, 0x920);
WWHD_OFFSET(Kk1EventState, mAnimationEnded, 0x922);
WWHD_OFFSET(Kk1EventState, mOnPath, 0x926);
WWHD_OFFSET(Kk1EventState, mByeComplete, 0x92A);
WWHD_OFFSET(Kk1EventState, mHideBody, 0x92B);
WWHD_OFFSET(Kk1EventState, mDemoSuspend, 0x92D);
WWHD_OFFSET(Kk1EventState, mEffectHidden, 0x92E);
WWHD_OFFSET(Kk1EventState, mPreserveShapeAngle, 0x92F);
WWHD_OFFSET(Kk1EventState, mHide, 0x930);
WWHD_OFFSET(Kk1EventState, mHomeInitialized, 0x931);
WWHD_OFFSET(Kk1EventState, mHeadOnlyFollow, 0x934);
WWHD_OFFSET(Kk1EventState, mInDemo, 0x936);
WWHD_OFFSET(Kk1EventState, mEffectFrame, 0xAAC);
WWHD_OFFSET(Kk1EventState, mEffectModel, 0xAB4);
WWHD_OFFSET(Kk1EventState, mCutIndex, 0xAC0);
WWHD_OFFSET(Kk1EventState, mMoveMode, 0xAC1);
WWHD_OFFSET(Kk1EventState, mCommentAction, 0xAC2);
WWHD_OFFSET(Kk1EventState, mAnmNum, 0xAC6);
WWHD_OFFSET(Kk1EventState, mOrderEvent, 0xAC7);
WWHD_OFFSET(Kk1EventState, mLookMode, 0xACA);
inline Kk1EventState& kk1Event(daNpc_Kk1_c* actor) {
    return *reinterpret_cast<Kk1EventState*>(actor);
}

// HD: original helper signatures retain boolean/signed-byte result conventions.
bool kk1PathNext(dNpc_PathRun_l* path);
bool kk1PathPrevious(dNpc_PathRun_l* path);
u8 kk1PathPointCount(dNpc_PathRun_l* path);
s8 kk1PathPointArg(dNpc_PathRun_l* path, u8 index);
bool kk1PathFindNearest(dNpc_PathRun_l* path, cXyz* pos, f32 radius);
BOOL kk1PathPassed(dNpc_PathRun_l* path, cXyz* pos, bool forward);
void kk1PathPoint(cXyz* result, dNpc_PathRun_l* path);
void kk1LookAtTarget(dNpc_JntCtrl_c* control, be<s16>* angle, cXyz* target,
                     cXyz* eye, s16 yaw, s16 velocity, u8 headOnly);
void kk1EntryColorAnimation(void* animation, J3DModelData* model, f32 frame);

struct Kk1PlayerVtable {
    u8 _reserved[0x114];
    gfn setPlayerPosAndAngle;
};
WWHD_OFFSET(Kk1PlayerVtable, setPlayerPosAndAngle, 0x114);
struct Kk1MaterialAnimations {
    u8 _reserved[0x38];
    gptr<void> mPattern;
    u8 _unused[4];
    gptr<void> mColor;
    gptr<void> mTextureSrt;
};
WWHD_OFFSET(Kk1MaterialAnimations, mTextureSrt, 0x44);
inline Kk1MaterialAnimations& kk1Animations(J3DModelData* model) {
    return *reinterpret_cast<Kk1MaterialAnimations*>(model);
}

struct Kk1EventParameters {
    u8 _reserved0[0xC];
    /* 0x0C */ be<s16> mHeadMinX;
    /* 0x0E */ be<s16> mHeadMaxX;
    /* 0x10 */ be<s16> mHeadMinY;
    /* 0x12 */ be<s16> mHeadMaxY;
    /* 0x14 */ be<s16> mBodyMinX;
    /* 0x16 */ be<s16> mBodyMaxX;
    /* 0x18 */ be<s16> mBodyMinY;
    /* 0x1A */ be<s16> mBodyMaxY;
    /* 0x1C */ be<s16> mMaxTurn;
    /* 0x1E */ be<s16> mLookVelocity;
    u8 _reserved1[0x4];
    /* 0x24 */ be<u8> mDebugDraw;
    u8 _reserved2[0x3];
    /* 0x28 */ be<s16> mLookTime;
    /* 0x2A */ be<s16> mLookDelay;
    u8 _reserved3[0x4];
    /* 0x30 */ be<s16> mTurnRate;
    /* 0x32 */ be<s16> mTurnStep;
    u8 _reserved4[0x10];
    /* 0x44 */ be<f32> mWalkSpeed;
    /* 0x48 */ be<f32> mWalkStep;
    /* 0x4C */ be<f32> mWalkRate;
    /* 0x50 */ be<f32> mRunSpeed;
    /* 0x54 */ be<f32> mRunStep;
    /* 0x58 */ be<f32> mRunRate;
    u8 _tail[4];
};
WWHD_SIZE(Kk1EventParameters, 0x60);
WWHD_OFFSET(Kk1EventParameters, mHeadMinX, 0xC);
WWHD_OFFSET(Kk1EventParameters, mHeadMaxX, 0xE);
WWHD_OFFSET(Kk1EventParameters, mHeadMinY, 0x10);
WWHD_OFFSET(Kk1EventParameters, mHeadMaxY, 0x12);
WWHD_OFFSET(Kk1EventParameters, mBodyMinX, 0x14);
WWHD_OFFSET(Kk1EventParameters, mBodyMaxX, 0x16);
WWHD_OFFSET(Kk1EventParameters, mBodyMinY, 0x18);
WWHD_OFFSET(Kk1EventParameters, mBodyMaxY, 0x1A);
WWHD_OFFSET(Kk1EventParameters, mMaxTurn, 0x1C);
WWHD_OFFSET(Kk1EventParameters, mLookVelocity, 0x1E);
WWHD_OFFSET(Kk1EventParameters, mDebugDraw, 0x24);
WWHD_OFFSET(Kk1EventParameters, mLookTime, 0x28);
WWHD_OFFSET(Kk1EventParameters, mLookDelay, 0x2A);
WWHD_OFFSET(Kk1EventParameters, mTurnRate, 0x30);
WWHD_OFFSET(Kk1EventParameters, mTurnStep, 0x32);
WWHD_OFFSET(Kk1EventParameters, mWalkSpeed, 0x44);
WWHD_OFFSET(Kk1EventParameters, mWalkStep, 0x48);
WWHD_OFFSET(Kk1EventParameters, mWalkRate, 0x4C);
WWHD_OFFSET(Kk1EventParameters, mRunSpeed, 0x50);
WWHD_OFFSET(Kk1EventParameters, mRunStep, 0x54);
WWHD_OFFSET(Kk1EventParameters, mRunRate, 0x58);
// HD: non-owning views of the shared HIO and two debug registers.
inline Kk1EventParameters& kk1Parameters() {
    return *gabi::at<Kk1EventParameters>(0x10467698);
}
struct Kk1PathDebug { be<s16> mOffsetX, mOffsetZ; };
WWHD_SIZE(Kk1PathDebug, 4);
inline Kk1PathDebug& kk1PathDebug() { return *gabi::at<Kk1PathDebug>(0x1047C76C); }

void kk1EventCutEnd(dEvent_manager_c* manager, s32 staff);
BOOL kk1EventAdvanced(dEvent_manager_c* manager, s32 staff);

u32 kk1EventActorId(dEvt_control_c* control, fopAc_ac_c* actor);

struct Kk1PlayerDispatch {
    u8 _reserved[0xB4];
    gptr<Kk1PlayerVtable> mVtable;
};
WWHD_OFFSET(Kk1PlayerDispatch, mVtable, 0xB4);
WWHD_OFFSET(Kk1EventState, mFrameFlags, 0x7D6);
enum Kk1Cut {
    KK1_CUT_RUN_START, KK1_CUT_RUN, KK1_CUT_CATCH_START, KK1_CUT_CATCH_END,
    KK1_CUT_TRN, KK1_CUT_BYE_START, KK1_CUT_BYE, KK1_CUT_BYE_END,
    KK1_CUT_PLYER_TRN, KK1_CUT_OTOBOKE, KK1_CUT_PLYER_MOV,
    KK1_CUT_RUNAWAY_START, KK1_CUT_RUNAWAY_END, KK1_CUT_BYE_CONTINUE,
};
