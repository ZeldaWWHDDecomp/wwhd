/* Joel and Zill animation, message, attention, path and particle slice.
 * GC supplies method names but no field layout; HD fopNpc base0x7DC, actor0xA1C.
 * Private measured layout leaves all other Ko1 slice headers unchanged. */
#pragma once
#include "d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/tu.h"
struct Ko1PathRun {
    gptr<dPath> mPath;
    be<u8> mFlags;
    be<u8> mIdx;
    be<u8> mbDir;
    be<u8> mState;
};
WWHD_SIZE(Ko1PathRun, 8);
struct Ko1RippleCallback {
    be<u32> __vtbl;
    be<u32> mEmitter;
    be<u32> mParam1;
    be<u32> mParam2;
    be<f32> mRate;
};
WWHD_SIZE(Ko1RippleCallback, 0x14);
struct Ko1ProcRecord { be<s16> mDelta; be<s16> mIndex; be<u32> mFunction; };
WWHD_SIZE(Ko1ProcRecord, 8);
struct daNpc_Ko1_c : fopNpc_npc_c {
    enum Status_e { STATUS_SWIM_1 = 7 };
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 mPad02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ u8 mPad10[4]; // HD: parameter records have one extra word.
    };

    /* ---- part C (d_a_npc_ko1_c.cpp) ---- */
    s32 anmNum_toResID(s32); 
    s32 headAnmNum_toResID(s32); 
    s32 balloon_anmNum_toResID(s32); 
    u32 setAnm_tex(s8); 
    BOOL setAnm_anm(anm_prm_c*); 
    BOOL set_balloonAnm_anm(anm_prm_c*); 
    BOOL set_balloonAnm_NUM(s32); 
    void setAnm_NUM(s32, s32); 
    bool setAnm(); 
    void chg_anmTag(); 
    void setAnm_ATR(s32); 
    void control_anmTag(); 
    void chg_anmAtr(u8); 
    void control_anmAtr(); 
    void anmAtr(u16); 
    bool chk_talk(); 
    fopAc_ac_c* searchByID(fpc_ProcID); 
    bool chk_manzai_1(); 
    u8 chk_partsNotMove(); 
    s8 bitCount(u8); 
    u16 next_msgStatus(be<u32>*); 
    u32 getMsg_HNA_0(); 
    u32 getMsg_HNA_1(); 
    u32 getMsg_HNA_2(); 
    u32 getMsg_HNA_3(); 
    u32 getMsg_BOU_0(); 
    u32 getMsg_BOU_1(); 
    u32 getMsg_BOU_2(); 
    u32 getMsg(); 
    u8 chkAttention(); 
    bool check_landOn(); 
    void ko_setPthPos(); 
    void set_tgtPos(cXyz* o_result, cXyz* i_pos); 
    void setPrtcl_Hamon(f32, f32); 
    bool chk_start_swim(); 
    fpc_ProcID get_crsActorID(); 
    bool chk_areaIn(f32, cXyz* i_pos); 
    void setPrtcl_HanaPachi(); 
    void clrSpd(); 
    u8 _7DC[0xD];
    /* 0x7E9 */ be<s8> mBalloonJoint;
    u8 _7EA[0x32];
    /* 0x81C */ gptr<mDoExt_McaMorf> mpBalloonMorf;
    u8 _820[0x4];
    /* 0x824 */ gptr<mDoExt_McaMorf> mpHeadMorf;
    u8 _828[0x84];
    /* 0x8AC */ Ko1PathRun mPathRun;
    u8 _8B4[0x70];
    /* 0x924 */ union { be<u32> mPartnerIds[3]; struct { u8 _00[8]; be<u8> mCount; u8 _09[3]; } mPartnerMetadata; };
    u8 _930[0x64];
    /* 0x994 */ be<f32> mAnimationFrame;
    /* 0x998 */ be<f32> mBalloonAnimationFrame;
    /* 0x99C */ be<f32> mMoveSpeed;
    u8 _9A0[0x4];
    /* 0x9A4 */ be<f32> mMoveAcceleration;
    u8 _9A8[0x8];
    /* 0x9B0 */ be<s16> mHeadAngleY;
    /* 0x9B2 */ be<s16> mBodyAngleY;
    /* 0x9B4 */ be<s16> mActorAngleY;
    u8 _9B6[0x2];
    /* 0x9B8 */ be<u32> mPartnerMsgNo;
    u8 _9BC[0xE];
    /* 0x9CA */ be<u16> mTargetOffsetAngle;
    u8 _9CC[0x2];
    /* 0x9CE */ be<u8> mBalloonAnimationState;
    /* 0x9CF */ be<u8> mbBalloonAnimationStopped;
    /* 0x9D0 */ be<s8> mAnimationEnded;
    /* 0x9D1 */ be<u8> mAnimationState;
    u8 _9D2[0x1];
    /* 0x9D3 */ be<u8> mMessageSelector;
    u8 _9D4[0x9];
    /* 0x9DD */ be<u8> mbTalkTriggered;
    u8 _9DE[0x8];
    /* 0x9E6 */ be<u8> mbMoving;
    u8 _9E7[0x1];
    /* 0x9E8 */ Ko1RippleCallback mRipple;
    /* 0x9FC */ be<u32> mRippleEmitter;
    /* 0xA00 */ be<u32> mBalloonEmitter;
    /* 0xA04 */ be<u32> mBalloonLightEmitter;
    u8 _A08[0x4];
    /* 0xA0C */ be<u8> mAnimationSelector;
    /* 0xA0D */ be<u8> mAnimationTag;
    /* 0xA0E */ be<s8> mBtpNum;
    /* 0xA0F */ be<s8> mAnimationNum;
    u8 _A10[0x1];
    /* 0xA11 */ be<s8> mBalloonAnimationNum;
    u8 _A12[0x1];
    /* 0xA13 */ be<s8> mStatus;
    u8 _A14[0x1];
    /* 0xA15 */ be<s8> mLookTarget;
    /* 0xA16 */ be<s8> mType;
    /* 0xA17 */ be<s8> mSpecificType;
    u8 _A18[0x1];
    /* 0xA19 */ be<s8> mbMessageTagsStarted;
    u8 _A1A[0x2];
};
WWHD_OFFSET(daNpc_Ko1_c, mBalloonJoint, 0x7E9);
WWHD_OFFSET(daNpc_Ko1_c, mpBalloonMorf, 0x81C);
WWHD_OFFSET(daNpc_Ko1_c, mpHeadMorf, 0x824);
WWHD_OFFSET(daNpc_Ko1_c, mPathRun, 0x8AC);
WWHD_OFFSET(daNpc_Ko1_c, mPartnerIds, 0x924);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationFrame, 0x994);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonAnimationFrame, 0x998);
WWHD_OFFSET(daNpc_Ko1_c, mMoveSpeed, 0x99C);
WWHD_OFFSET(daNpc_Ko1_c, mMoveAcceleration, 0x9A4);
WWHD_OFFSET(daNpc_Ko1_c, mHeadAngleY, 0x9B0);
WWHD_OFFSET(daNpc_Ko1_c, mBodyAngleY, 0x9B2);
WWHD_OFFSET(daNpc_Ko1_c, mActorAngleY, 0x9B4);
WWHD_OFFSET(daNpc_Ko1_c, mPartnerMsgNo, 0x9B8);
WWHD_OFFSET(daNpc_Ko1_c, mTargetOffsetAngle, 0x9CA);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonAnimationState, 0x9CE);
WWHD_OFFSET(daNpc_Ko1_c, mbBalloonAnimationStopped, 0x9CF);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationEnded, 0x9D0);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationState, 0x9D1);
WWHD_OFFSET(daNpc_Ko1_c, mMessageSelector, 0x9D3);
WWHD_OFFSET(daNpc_Ko1_c, mbTalkTriggered, 0x9DD);
WWHD_OFFSET(daNpc_Ko1_c, mbMoving, 0x9E6);
WWHD_OFFSET(daNpc_Ko1_c, mRipple, 0x9E8);
WWHD_OFFSET(daNpc_Ko1_c, mRippleEmitter, 0x9FC);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonEmitter, 0xA00);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonLightEmitter, 0xA04);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationSelector, 0xA0C);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationTag, 0xA0D);
WWHD_OFFSET(daNpc_Ko1_c, mBtpNum, 0xA0E);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationNum, 0xA0F);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonAnimationNum, 0xA11);
WWHD_OFFSET(daNpc_Ko1_c, mStatus, 0xA13);
WWHD_OFFSET(daNpc_Ko1_c, mLookTarget, 0xA15);
WWHD_OFFSET(daNpc_Ko1_c, mType, 0xA16);
WWHD_OFFSET(daNpc_Ko1_c, mSpecificType, 0xA17);
WWHD_OFFSET(daNpc_Ko1_c, mbMessageTagsStarted, 0xA19);
WWHD_SIZE(daNpc_Ko1_c, 0xA1C);
WWHD_SIZE(daNpc_Ko1_c::anm_prm_c, 0x14);

struct Ko1TalkPlayView {
    u8 _00[0x5C34];
    be<u32> mIndexAndActors[11];
};
WWHD_OFFSET(Ko1TalkPlayView, mIndexAndActors, 0x5C34);
WWHD_SIZE(Ko1TalkPlayView, 0x5C60);
inline Ko1TalkPlayView* Ko1_talkPlay(u32 address) { return gabi::at<Ko1TalkPlayView>(address); }
struct Ko1PlayerView { u8 _00[0x3C0]; be<u32> mGroundFlags; };
WWHD_OFFSET(Ko1PlayerView, mGroundFlags, 0x3C0);
WWHD_SIZE(Ko1PlayerView, 0x3C4);
struct Ko1CollisionObject { u8 _00[0x44]; gptr<dCcD_Stts> mpStts; };
WWHD_OFFSET(Ko1CollisionObject, mpStts, 0x44);
WWHD_SIZE(Ko1CollisionObject, 0x48);

Ko1CollisionObject* Ko1_getCoHitObj(dCcD_GObjInf* obj);
void Ko1_pathPoint(Ko1PathRun* path, cXyz* out, u8 index);
BOOL Ko1_nextPathIndex(Ko1PathRun* path);
void Ko1_endRipple(Ko1RippleCallback* ripple);
u32 Ko1_initTextureAnimation(daNpc_Ko1_c* actor, u32 enabled);
void Ko1_findActor(fpc_ProcID id, gptr<fopAc_ac_c>* out);
bool Ko1_lockonTruth(dAttention_c* attention);
fopAc_ac_c* Ko1_lockonTarget(dAttention_c* attention, s32 index);
fopAc_ac_c* Ko1_actionTarget(dAttention_c* attention, s32 index);
