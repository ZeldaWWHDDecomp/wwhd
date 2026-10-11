// Possessed/game-over camera work.
#pragma once
#include "d/d_camera.h"
struct PossessedCameraWork {
    be<u32> mState, mType, mTimer, mFramesLeft;
    be<f32> mRadius;
    cSAngle_l mLatitude, mLongitude;
    be<f32> mFovy, mCushion;
    be<u32> mBlure;
    gptr<fopAc_ac_c> mpTarget;
    cSGlobe_l mTargetDirection;
};
WWHD_OFFSET(PossessedCameraWork, mpTarget, 0x24);
WWHD_OFFSET(PossessedCameraWork, mTargetDirection, 0x28);
WWHD_SIZE(PossessedCameraWork, 0x30);
struct GameOverCameraWork {
    be<u32> mState, mTimer, mMirror;
    cXyz mCenterOffset, mEyeOffset;
};
WWHD_SIZE(GameOverCameraWork, 0x24);
// HD: the last eye offset aliases GHS's saved r25/r26/r27 words.
struct GameOverCameraFrame {
    u8 _00[8];
    cSAngle_l mAngle, mCopyAngleA;
    cXyz mEye;
    u8 _18[0xC];
    cSAngle_l mCopyAngleB, mApplyAngle;
    cXyz mCloseEye, mCloseEyeOffset;
    u8 _40[0xC];
    cSAngle_l mCloseAngle;
    u8 _4E[2];
    cXyz mCloseCenterOffset, mInitialCenter;
    u8 _68[0x18];
    cXyz mInitialCenterOffset, mCloseCenter;
    u8 _98[0x48];
    union {
        cXyz mEyeOffsets[6];
        struct {
            cXyz _firstFive[5];
            be<u32> mSavedRegisters[3];
        } mSave;
    } mSearch;
    u8 _128[0x28];
};
WWHD_OFFSET(GameOverCameraFrame, mCloseEye, 0x28);
WWHD_OFFSET(GameOverCameraFrame, mCloseEyeOffset, 0x34);
WWHD_OFFSET(GameOverCameraFrame, mCloseAngle, 0x4C);
WWHD_OFFSET(GameOverCameraFrame, mCloseCenterOffset, 0x50);
WWHD_OFFSET(GameOverCameraFrame, mInitialCenter, 0x5C);
WWHD_OFFSET(GameOverCameraFrame, mInitialCenterOffset, 0x80);
WWHD_OFFSET(GameOverCameraFrame, mCloseCenter, 0x8C);
WWHD_OFFSET(GameOverCameraFrame, mSearch, 0xE0);
WWHD_SIZE(GameOverCameraFrame, 0x150);
struct PossessedEventCamera_c : dCamera_c {
    u32 possessedEvCamera();
    u32 gameOverEvCamera();
};
WWHD_SIZE(PossessedEventCamera_c, 0x8E0);

// HD: explicit GHS hidden-result bindings, unique to this split source.
u32 ev7_getEvIntData(u32 c, u32 out, u32 name, s32 def);
u32 ev7_getEvFloatData(u32 c, u32 out, u32 name, f32 def);
u32 ev7_getEvActor2(u32 c, u32 name, u32 def);
void ev7_eyePos(u32 c, u32 out, u32 actor);
void ev7_directionOf(u32 c, u32 out, u32 actor);
void ev7_cXyz_mi(u32 a, u32 out, u32 b);
void ev7_cXyz_pl(u32 a, u32 out, u32 b);
void ev7_cXyz_ml(u32 a, u32 out, f32 s);
void ev7_vecAdd(u32 a, u32 b, u32 out);
void ev7_cSGlobe_Val(u32 g, u32 v);
void ev7_cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u);
void ev7_cSGlobe_Xyz(u32 g, u32 out);
void ev7_cSAngle_mi(u32 a, u32 out, u32 b);
void ev7_cSAngle_ml(u32 a, u32 out, f32 f);
void ev7_cSAngle_pl(u32 a, u32 out, u32 b);
u32 ev7_cSAngleS(u32 tmp, s16 v);
void ev7_positionOf(u32 c, u32 out, u32 actor);
void ev7_relationalPos(u32 c, u32 out, u32 actor, u32 ofs);
void ev7_relationalPosA(u32 c, u32 out, u32 actor, u32 ofs, u32 ang);
bool ev7_lineBGCheck(u32 c, u32 a, u32 b, u32 flags);
u32 ev7_cSAngleCopy(u32 out, u32 src);
void ev7_ResetBlure(u32, u32);
void ev7_SetBlurePositionType(u32, u32);
void ev7_SetBlureTimer(u32, u32);
void ev7_SetBlureAlpha(u32, f32);
void ev7_SetBlureScaleUniform(u32, f32);
void ev7_StartShock(u32, s32, s32, u32);
void ev7_cSAngle_Val(u32, f32);
s32 ev7_GetCameraParam(u32);
void ev7_project(u32, u32);
void ev7_SetBlurePosition(u32, f32, f32, f32);
void ev7_SetBlureScale(u32, f32, f32, f32);
void ev7_cSAngle_default(u32);

// HD: display-list window dimensions reached through play.mDlstWindow.
struct EventCameraWindowHD {
    u8 _00[0x20];
    be<f32> mWidth, mHeight;
    u8 _28[4];
};
WWHD_SIZE(EventCameraWindowHD, 0x2C);

WWHD_OFFSET(EventCameraWindowHD, mWidth, 0x20);
WWHD_OFFSET(EventCameraWindowHD, mHeight, 0x24);
