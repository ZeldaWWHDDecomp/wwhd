// Conversation-camera work and HD shot data.
#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
/* work area of talktoCamera (mWork) */
struct TalkWork {
    /* 0x00 */ be<u32> mTag;              /* 'TALK' */
    /* 0x04 */ cXyz mCenter;              /* centre */
    /* 0x10 */ cXyz mEye;                 /* eye */
    /* 0x1C */ cXyz mShotCenter;          /* shot centre / height offset (y) */
    /* 0x28 */ cSGlobe_l mDirection;      /* direction */
    /* 0x30 */ cSGlobe_l mActorDirection; /* speaker -> listener */
    /* 0x38 */ be<s32> mSide;             /* side (-1: not chosen) */
    /* 0x3C */ be<s32> mShotType;         /* shot type (HD table index) */
    /* 0x40 */ be<s32> mPreviousShotType; /* previous shot type */
    /* 0x44 */ be<s32> mShotFrames;       /* frames in this shot */
    /* 0x48 */ be<s32> mTransitionFrames; /* transition frames */
    /* 0x4C */ be<f32> mWeightSum;        /* remaining weight sum */
    /* 0x50 */ be<f32> mFrameWeight;
    /* 0x54 */ u8 _54[4];
    /* 0x58 */ be<f32> mRadius;                    /* Radius */
    /* 0x5C */ be<f32> mFovy;                      /* Fovy */
    /* 0x60 */ be<f32> mLongitude;                 /* Longitude (999.9: free) */
    /* 0x64 */ be<f32> mLatitude;                  /* Latitude */
    /* 0x68 */ gptr<fopAc_ac_c> mpListener;        /* Listener */
    /* 0x6C */ gptr<fopAc_ac_c> mpSpeaker;         /* Speaker */
    /* 0x70 */ gptr<fopAc_ac_c> mpPreviousSpeaker; /* last speaker */
    /* 0x74 */ be<s16> mSmoothless;                /* Smoothless */
    /* 0x76 */ be<s16> mMode;                      /* Mode */
};
WWHD_SIZE(TalkWork, 0x78);

// HD: the event manager's camera-play selector.
struct TalkEventManagerHD {
    u8 _00[0x20];
    be<s32> mCameraPlay;
};
struct TalkEventNames {
    char mSpeaker[8];
    char mMode[8];
    char mRadius[8];
    char mFovy[8];
    char mLongitude[12];
    char mSmoothless[12];
    char mLatitude[12];
    char mListener[12];
    char mStarter[12];
    char mTalkPartner[16];
};
WWHD_SIZE(TalkEventNames, 0x6C);
namespace talkDetail {
// HD: actor profile numbers and conversation shot table.
enum { fpcNm_SHIP_e = 0xA5, fpcNm_KANBAN_e = 0xB6, TALK_WORK_TAG = 0x54414C4B };
enum TalkShot {
    TALK_PAIR = 0,
    TALK_CLOSEUP_A = 11,
    TALK_CLOSEUP_B = 12,
    TALK_WIDE = 13,
    TALK_EYE_A = 14,
    TALK_EYE_B = 15,
    TALK_SHOULDER_NEAR_A = 16,
    TALK_SHOULDER_NEAR_B = 17,
    TALK_SIDE_A = 18,
    TALK_SIDE_B = 19,
    TALK_SHOULDER_A = 20,
    TALK_SHOULDER_B = 21,
    TALK_SHOULDER_LOW_A = 22,
    TALK_SHOULDER_LOW_B = 23,
    TALK_SIDE_HIGH_A = 24,
    TALK_SIDE_HIGH_B = 25,
    TALK_SIDE_ATTN_A = 26,
    TALK_SIDE_ATTN_B = 27,
    TALK_MIDPOINT = 28,
    TALK_SHOULDER_ATTN = 29,
    TALK_CLOSEUP_WIDE_A = 30,
    TALK_CLOSEUP_WIDE_B = 31,
};
} // namespace talkDetail
cSAngle_l* talk_angleFromShort(cSAngle_l*, s16);
void talk_angleSubtract(const void*, cSAngle_l*, const void*);
void talk_angleMultiply(const void*, cSAngle_l*, f32);
void talk_angleAdd(const void*, cSAngle_l*, const void*);
f32 talk_angleCos(const void*);
bool talk_ChkCamera(void*, cXyz*, cXyz*, f32, fopAc_ac_c*, fopAc_ac_c*);
void talk_globeXyz(cSGlobe_l*, cXyz*);
void talk_globeFromVector(cSGlobe_l*, cXyz*);
cSAngle_l* talk_angleFromDegrees(cSAngle_l*, f32);
void talk_globeVal(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
void talk_getEvIntData(dCamera_c*, be<s32>*, const char*, s32);
void talk_getEvFloatData(dCamera_c*, be<f32>*, const char*, f32);
fopAc_ac_c* talk_getEvActor(dCamera_c*, const char*, const char*);
u32 talk_getTableEntryHD();
s32 talk_getTableIndexHD();
void talk_relationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, fopAc_ac_c*, cXyz*, f32);
void talk_angleValDegrees(cSAngle_l*, f32);
void talk_angleNegate(cSAngle_l*, cSAngle_l*);
f32 talk_angleDegree(cSAngle_l*);
bool talk_lineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
void talk_angleAddShort(cSAngle_l*, cSAngle_l*, s16);
