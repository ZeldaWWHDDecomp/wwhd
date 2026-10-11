/* Joel and Zill movement/state slice.
 * GC has only names/nonmatching stubs and no member layout. HD fopNpc base is0x7DC,
 * actor size0xA1C; fields below are measured and named for their observed use.
 * This private slice header leaves the shared Ko1 header and other parts unchanged. */
#pragma once
#include "d/d_npc.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "SSystem/SComponent/c_counter.h"

struct daNpc_Ko1_c : fopNpc_npc_c {
    enum Status_e {
        STATUS_WAIT_1 = 1,
        STATUS_WAIT_2 = 2,
        STATUS_TALK = 3,
        STATUS_WALK_1 = 4,
        STATUS_WAIT_3 = 5,
        STATUS_ATTACK_1 = 6,
        STATUS_SWIM_1 = 7,
        STATUS_SWIM_2 = 8,
        STATUS_DOWN_1 = 9,
        STATUS_WAIT_4 = 10,
        STATUS_WALK_3 = 11,
        STATUS_WAIT_5 = 12,
        STATUS_ATTACK_2 = 13,
        STATUS_WAIT_6 = 14,
        STATUS_WALK_2 = 15,
        STATUS_DOWN_2 = 16,
        STATUS_ATTACK_3 = 17,
        STATUS_DOWN_3 = 18,
        STATUS_WAIT_7 = 19,
        STATUS_MANZAI = 20,
        STATUS_TALK_END = 21,
        STATUS_WAIT_5_ALT = 22,
        STATUS_ATTACK_2_ALT = 23,
        STATUS_WALK_2_ALT = 24,
        STATUS_DOWN_ALT = 25,
        STATUS_WAIT_9 = 26,
        STATUS_SLEEP_1 = 27,
        STATUS_SLEEP_2 = 28,
        STATUS_WAIT_A = 29,
    };
    void setStt(s8 i_status);
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL wait_4();
    BOOL wait_5(s8 i_nextStatus);
    BOOL wait_6();
    BOOL wait_7();
    BOOL wait_9();
    BOOL wait_a();
    BOOL walk_1();
    BOOL walk_2(s8 i_status1, s8 i_status2);
    BOOL walk_3();
    BOOL swim_1();
    BOOL swim_2();
    BOOL attk_1();
    BOOL attk_2(s8 i_status1, s8 i_status2);
    BOOL attk_3();
    BOOL down_1(s8 i_status);
    u8 _7DC[0xD0];
    /* 0x8AC */ dNpc_PathRun_c mPathRun;
    /* 0x8B4 */ be<u32> mSavedPath;
    u8 _8B8[0x6C];
    /* 0x924 */ be<u32> mPartnerId;
    u8 _928[0x8];
    /* 0x930 */ be<u32> mAttackActorId;
    /* 0x934 */ cXyz mInitialPos;
    /* 0x940 */ csXyz mInitialAngle;
    u8 _946[0x12];
    /* 0x958 */ cXyz mLookPos;
    /* 0x964 */ cXyz mTargetPos;
    u8 _970[0xC];
    /* 0x97C */ cXyz mAreaCenter;
    u8 _988[0x14];
    /* 0x99C */ be<f32> mMoveSpeed;
    /* 0x9A0 */ be<f32> mVerticalSpeed;
    /* 0x9A4 */ be<f32> mMoveAcceleration;
    u8 _9A8[0x4];
    /* 0x9AC */ be<f32> mTurnRate;
    u8 _9B0[0xE];
    /* 0x9BE */ be<s16> mTalkWaitTimer;
    u8 _9C0[0x2];
    /* 0x9C2 */ be<s16> mWaitTimer;
    /* 0x9C4 */ be<s16> mBalloonTimer;
    u8 _9C6[0x2];
    /* 0x9C8 */ be<s16> mTurnAngle;
    u8 _9CA[0x6];
    /* 0x9D0 */ be<u8> mAnimationEnded;
    u8 _9D1[0x4];
    /* 0x9D5 */ be<u8> mSleepState;
    u8 _9D6[0x5];
    /* 0x9DB */ be<u8> mbMoveFinished;
    /* 0x9DC */ be<u8> mbJumping;
    u8 _9DD[0x7];
    /* 0x9E4 */ be<u8> mAttention;
    /* 0x9E5 */ be<u8> mTalkFlag;
    /* 0x9E6 */ be<u8> mbMoving;
    u8 _9E7[0x21];
    /* 0xA08 */ be<u8> mMoveMode;
    /* 0xA09 */ be<u8> mPreviousMoveMode;
    u8 _A0A[0x2];
    /* 0xA0C */ be<u8> mAnimationSelector;
    /* 0xA0D */ be<u8> mHeadAnimationSelector;
    u8 _A0E[0x1];
    /* 0xA0F */ be<u8> mAnimationNum;
    u8 _A10[0x2];
    /* 0xA12 */ be<s8> mLookMode;
    /* 0xA13 */ be<s8> mStatus;
    /* 0xA14 */ be<s8> mResumeStatus;
    /* 0xA15 */ be<s8> mLookTarget;
    /* 0xA16 */ be<s8> mType;
    u8 _A17[0x5];
};
WWHD_OFFSET(daNpc_Ko1_c, mPathRun, 0x8AC);
WWHD_OFFSET(daNpc_Ko1_c, mSavedPath, 0x8B4);
WWHD_OFFSET(daNpc_Ko1_c, mPartnerId, 0x924);
WWHD_OFFSET(daNpc_Ko1_c, mAttackActorId, 0x930);
WWHD_OFFSET(daNpc_Ko1_c, mInitialPos, 0x934);
WWHD_OFFSET(daNpc_Ko1_c, mInitialAngle, 0x940);
WWHD_OFFSET(daNpc_Ko1_c, mLookPos, 0x958);
WWHD_OFFSET(daNpc_Ko1_c, mTargetPos, 0x964);
WWHD_OFFSET(daNpc_Ko1_c, mAreaCenter, 0x97C);
WWHD_OFFSET(daNpc_Ko1_c, mMoveSpeed, 0x99C);
WWHD_OFFSET(daNpc_Ko1_c, mVerticalSpeed, 0x9A0);
WWHD_OFFSET(daNpc_Ko1_c, mMoveAcceleration, 0x9A4);
WWHD_OFFSET(daNpc_Ko1_c, mTurnRate, 0x9AC);
WWHD_OFFSET(daNpc_Ko1_c, mTalkWaitTimer, 0x9BE);
WWHD_OFFSET(daNpc_Ko1_c, mWaitTimer, 0x9C2);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonTimer, 0x9C4);
WWHD_OFFSET(daNpc_Ko1_c, mTurnAngle, 0x9C8);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationEnded, 0x9D0);
WWHD_OFFSET(daNpc_Ko1_c, mSleepState, 0x9D5);
WWHD_OFFSET(daNpc_Ko1_c, mbMoveFinished, 0x9DB);
WWHD_OFFSET(daNpc_Ko1_c, mbJumping, 0x9DC);
WWHD_OFFSET(daNpc_Ko1_c, mAttention, 0x9E4);
WWHD_OFFSET(daNpc_Ko1_c, mTalkFlag, 0x9E5);
WWHD_OFFSET(daNpc_Ko1_c, mbMoving, 0x9E6);
WWHD_OFFSET(daNpc_Ko1_c, mMoveMode, 0xA08);
WWHD_OFFSET(daNpc_Ko1_c, mPreviousMoveMode, 0xA09);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationSelector, 0xA0C);
WWHD_OFFSET(daNpc_Ko1_c, mHeadAnimationSelector, 0xA0D);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationNum, 0xA0F);
WWHD_OFFSET(daNpc_Ko1_c, mLookMode, 0xA12);
WWHD_OFFSET(daNpc_Ko1_c, mStatus, 0xA13);
WWHD_OFFSET(daNpc_Ko1_c, mResumeStatus, 0xA14);
WWHD_OFFSET(daNpc_Ko1_c, mLookTarget, 0xA15);
WWHD_OFFSET(daNpc_Ko1_c, mType, 0xA16);
WWHD_SIZE(daNpc_Ko1_c, 0xA1C);
// HD: shared HIO storage is owned by the main Ko1 translation unit.
struct daNpc_Ko1_childHIO_c {
    u8 _00[0x28];
    /* 0x28 */ be<f32> mWalkSpeed;
    /* 0x2C */ be<f32> mWalkAcceleration;
    /* 0x30 */ be<f32> mWalkTurnRate;
    u8 _34[0x4];
    /* 0x38 */ be<f32> mRunSpeed;
    /* 0x3C */ be<f32> mRunAcceleration;
    /* 0x40 */ be<f32> mRunTurnRate;
    /* 0x44 */ be<f32> mSwimTurnRate;
    /* 0x48 */ be<f32> mNearAreaRadius;
    /* 0x4C */ be<f32> mOuterAreaRadius;
    /* 0x50 */ be<f32> mWaitAreaRadius;
    /* 0x54 */ be<f32> mWalkAreaRadius;
    /* 0x58 */ be<f32> mSwimAreaRadius;
    u8 _5C[4];
};
WWHD_SIZE(daNpc_Ko1_childHIO_c, 0x60);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mWalkSpeed, 0x28);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mWalkAcceleration, 0x2C);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mWalkTurnRate, 0x30);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mRunSpeed, 0x38);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mRunAcceleration, 0x3C);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mRunTurnRate, 0x40);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mSwimTurnRate, 0x44);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mNearAreaRadius, 0x48);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mOuterAreaRadius, 0x4C);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mWaitAreaRadius, 0x50);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mWalkAreaRadius, 0x54);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mSwimAreaRadius, 0x58);
inline daNpc_Ko1_childHIO_c* Ko1_hioChildren() { return gabi::at<daNpc_Ko1_childHIO_c>(0x104677E8); }
inline counter_class& Ko1_counter() { return gabi::object_at<counter_class>(0x101FF558); }
enum Ko1Process_e { fpcNm_ITEM_e=0xA8, KO1_ATTACK_TARGET_PROCESS_e=0x140 };
fopAc_ac_c* Ko1_searchByID(daNpc_Ko1_c* actor, u32 id);
bool Ko1_chk_talk(daNpc_Ko1_c* actor);
s32 Ko1_chk_manzai_1(daNpc_Ko1_c* actor);
bool Ko1_chk_start_swim(daNpc_Ko1_c* actor);
u32 Ko1_get_crsActorID(daNpc_Ko1_c* actor);
bool Ko1_chk_areaIn(daNpc_Ko1_c* actor, f32 r, cXyz* pos);
void Ko1_set_tgtPos(daNpc_Ko1_c* actor, cXyz* out, cXyz* pos);
void Ko1_clrSpd(daNpc_Ko1_c* actor);
void Ko1_setAnm(daNpc_Ko1_c* actor);
void Ko1_setAnm_NUM(daNpc_Ko1_c* actor, s32 num, s32 tex);
s16 Ko1_calcTimer(be<s16>* timer);
s32 Ko1_getRndValue(s32 base, s32 range);
inline dPath* Ko1_savedPath(u32 address) { return gabi::at<dPath>(address); }
