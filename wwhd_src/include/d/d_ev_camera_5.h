/** Item acquisition, actor turn and saved-view restoration event cameras.
 * Private HD view: GameCube fields retain their names, mWork shifts by four bytes.
 * GameCube function bodies are nonmatching stubs. */
#pragma once
#include "gc_api/gc_api.h"
namespace event_camera_transition {
enum : u32 {
    CAMERA_HOLD_VIEW = 1, CAMERA_LINE_DEFAULT = 0x8F, PLAYER_HOLD_ITEM = 0x10000,
    fpcNm_SHIP_e = 0xA5, JUDGE_FOR_PROCESS_NAME = 0x025E121C,
    ITEM_WAIT = 1, ITEM_CHOOSE_EYE = 10, ITEM_BLEND = 11, ITEM_SETTLE = 12, ITEM_HOLD = 99,
    RESTORE_NEAR = 1, RESTORE_BLEND = 2, RESTORE_JUMP = 3, RESTORE_PLAY_SLOT = 9
};
struct CameraGlobeHD { be<f32> mRadius; be<s16> mV, mU; };
WWHD_SIZE(CameraGlobeHD, 8);
struct CameraViewHD {
    CameraGlobeHD mDirection;
    cXyz mCenter, mEye;
    be<s16> mBank;
    u8 mPadding22[2];
    be<f32> mFovy;
};
WWHD_SIZE(CameraViewHD, 0x28);
struct CameraSavedViewHD {
    cXyz mCenter, mEye;
    be<f32> mFovY;
    be<s16> mBank, m1E;
};
WWHD_SIZE(CameraSavedViewHD, 0x20);
struct ItemCameraWorkHD {
    be<u32> mState, mTimer;
    u8 mPadding8[4];
    cXyz mCenter;
    be<f32> mFovy;
    be<u32> mInitialCounter, mElapsed;
    CameraGlobeHD mDirection;
    u8 mPadding2C[0x44 - 0x2C];
    be<u32> mTimer1, mTimer2;
};
WWHD_SIZE(ItemCameraWorkHD, 0x4C);
WWHD_OFFSET(ItemCameraWorkHD, mDirection, 0x24);
WWHD_OFFSET(ItemCameraWorkHD, mTimer1, 0x44);
struct TurnCameraWorkHD {
    cXyz mCenterGap, mTargetPosition, mCenter;
    be<f32> mCushion;
    u8 mPadding28[4];
    be<u32> mTimer;
    gptr<fopAc_ac_c> mpTargetActor;
    u8 mPadding34[0x44 - 0x34];
    CameraGlobeHD mDirection;
    u8 mPadding4C[4];
    be<f32> mFrontAngle;
};
WWHD_SIZE(TurnCameraWorkHD, 0x54);
WWHD_OFFSET(TurnCameraWorkHD, mDirection, 0x44);
struct RestoreCameraWorkHD {
    cXyz mCenterGap, mTargetCenter;
    be<f32> mCushion;
    be<u32> mNearTimer;
    be<f32> mNearDist;
    be<u32> mFarTimer;
    be<f32> mFarDist;
    u8 mPadding2C[8];
    gptr<fopAc_ac_c> mpTargetActor;
    CameraGlobeHD mDirection;
    be<u32> mMode, mDest;
    CameraSavedViewHD mSavedView;
    be<u32> mTargetType;
};
WWHD_SIZE(RestoreCameraWorkHD, 0x6C);
WWHD_OFFSET(RestoreCameraWorkHD, mSavedView, 0x48);
struct dCamera_c {
    bool getItemEvCamera();
    bool turnToActorEvCamera();
    bool restorePosEvCamera();
    /* 0x000 */ u8 mPadding0[0x1C];
    /* 0x01C */ cXyz mEye;
    /* 0x028 */ u8 mPadding28[0x3C - 0x28];
    /* 0x03C */ CameraViewHD mViewCache;
    /* 0x064 */ u8 mPadding64[0x84 - 0x64];
    /* 0x084 */ cXyz m084;
    /* 0x090 */ cXyz m090;
    /* 0x09C */ be<f32> m09C;
    /* 0x0A0 */ u8 mPaddingA0[4];
    /* 0x0A4 */ CameraSavedViewHD m0A4[2];
    /* 0x0E4 */ u8 mPaddingE4[0x100 - 0xE4];
    /* 0x100 */ be<u8> m100, m101, m102;
    /* 0x103 */ u8 mPadding103[0x11C - 0x103];
    /* 0x11C */ be<u32> m11C;
    /* 0x120 */ u8 mPadding120[4];
    /* 0x124 */ be<s32> mPadId;
    /* 0x128 */ gptr<fopAc_ac_c> mpPlayerActor;
    /* 0x12C */ u8 mPadding12C[0x36C - 0x12C];
    /* 0x36C */ be<f32> m368;
    /* 0x370 */ u8 mPadding370[0x37C - 0x370];
    /* 0x37C */ union { ItemCameraWorkHD item; TurnCameraWorkHD turn; RestoreCameraWorkHD restore; } mWork;
    /* 0x3E8 */ u8 mPadding3E8[0x510 - 0x3E8];
    /* 0x510 */ be<u32> mEventFlags;
    /* 0x514 */ u8 mPadding514[0x8E0 - 0x514];
};
WWHD_OFFSET(dCamera_c, mViewCache.mCenter, 0x44);
WWHD_OFFSET(dCamera_c, mViewCache.mEye, 0x50);
WWHD_OFFSET(dCamera_c, mViewCache.mDirection.mV, 0x40);
WWHD_OFFSET(dCamera_c, mViewCache.mDirection.mU, 0x42);
WWHD_OFFSET(dCamera_c, mViewCache.mFovy, 0x60);
WWHD_OFFSET(dCamera_c, m084, 0x84);
WWHD_OFFSET(dCamera_c, m0A4, 0xA4);
WWHD_OFFSET(dCamera_c, m100, 0x100);
WWHD_OFFSET(dCamera_c, m11C, 0x11C);
WWHD_OFFSET(dCamera_c, mPadId, 0x124);
WWHD_OFFSET(dCamera_c, mpPlayerActor, 0x128);
WWHD_OFFSET(dCamera_c, m368, 0x36C);
WWHD_OFFSET(dCamera_c, mWork, 0x37C);
WWHD_OFFSET(dCamera_c, mEventFlags, 0x510);
WWHD_SIZE(dCamera_c, 0x8E0);
struct ItemAngleSlotsHD { be<s16> mProcessName, mU; };
WWHD_SIZE(ItemAngleSlotsHD, 4);
struct CameraWordsHD { be<u32> mWords[6]; };
WWHD_SIZE(CameraWordsHD, 0x18);
struct CameraPointWordsHD { be<u32> x, y, z; };
struct CameraDefaultGapsHD {
    cXyz mRestoreGap, mTurnGap;
    u8 mPadding18[0x2C - 0x18];
    be<u32> mRestoreGuard, mTurnGuard;
};
WWHD_OFFSET(CameraDefaultGapsHD, mRestoreGuard, 0x2C);
WWHD_OFFSET(CameraDefaultGapsHD, mTurnGuard, 0x30);
WWHD_SIZE(CameraDefaultGapsHD, 0x34);
struct CameraTimersHD { be<s32> mNearTimer, mFarTimer; u8 mPadding8[4]; be<s32> mTurnTimer; };
WWHD_SIZE(CameraTimersHD, 0x10);
struct PlaySavedCameraHD { u8 mPadding0[0x5B0C]; CameraSavedViewHD mSavedView; };
WWHD_OFFSET(PlaySavedCameraHD, mSavedView, 0x5B0C);
WWHD_SIZE(PlaySavedCameraHD, 0x5B2C);
} // namespace event_camera_transition

using namespace event_camera_transition;
void cameraTransitionAngleDefault(be<s16>*);
be<s16>* cameraTransitionAngleDegrees(be<s16>*,f32);
be<s16>* cameraTransitionAngleShort(be<s16>*,s32);
void cameraTransitionAngleSubtract(const be<s16>*,be<s16>*,const be<s16>*);
void cameraTransitionAngleMultiply(const be<s16>*,be<s16>*,f32);
void cameraTransitionAngleAddAssign(be<s16>*,const be<s16>*);
void cameraTransitionAngleAdd(const be<s16>*,be<s16>*,const be<s16>*);
void cameraTransitionSubtract(const cXyz*,cXyz*,const cXyz*);
void cameraTransitionAdd(const cXyz*,cXyz*,const cXyz*);
void cameraTransitionScale(const cXyz*,cXyz*,f32);
void cameraTransitionNormalize(const cXyz*,cXyz*);
f32 cameraTransitionSquareMagnitude(const cXyz*);
f32 cameraTransitionSquareRoot(f32);
void cameraTransitionGlobeVector(CameraGlobeHD*,const cXyz*);
void cameraTransitionGlobeFromVector(CameraGlobeHD*,const cXyz*);
void cameraTransitionGlobeXyz(const CameraGlobeHD*,cXyz*);
void cameraTransitionGlobeSet(CameraGlobeHD*,f32,const be<s16>*,const be<s16>*);
void cameraTransitionRelational(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*,const cXyz*);
void cameraTransitionPosition(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*);
void cameraTransitionAttention(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*);
void cameraTransitionDirection(event_camera_transition::dCamera_c*,be<s16>*,fopAc_ac_c*);
bool cameraTransitionLineCheck(event_camera_transition::dCamera_c*,const cXyz*,const cXyz*,u32);
bool cameraTransitionCheckCamera(cCcS*,cXyz*,cXyz*,f32,fopAc_ac_c*,fopAc_ac_c*);
u32 cameraTransitionGetInt(event_camera_transition::dCamera_c*,be<u32>*,const char*,s32);
u32 cameraTransitionGetFloat(event_camera_transition::dCamera_c*,be<f32>*,const char*,f32);
u32 cameraTransitionGetXyz(event_camera_transition::dCamera_c*,cXyz*,const char*,const cXyz*);
fopAc_ac_c* cameraTransitionGetActor(event_camera_transition::dCamera_c*,const char*,const char*);
u32 cameraTransitionPointInSight(event_camera_transition::dCamera_c*,const cXyz*);
inline f32 cameraTransitionVectorLength(const cXyz* p) { return cameraTransitionSquareRoot(cameraTransitionSquareMagnitude(p)); }
