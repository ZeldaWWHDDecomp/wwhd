/** Private camera utility views. HD offsets and widths preserve the verified layout. */
#pragma once
#include "d/d_camera.h"
namespace camera_util {
struct dCamForcusLine : ::dCamForcusLine { void Init(); void Draw(); };
WWHD_SIZE(dCamForcusLine, 0x70);
}
enum : u32 {
 CAMERA_STICK_USELESS=0x1000000, CAMERA_CSTICK_USELESS=0x800000,
 ATTENTION_LOCKON=0x20000000, SHAKE_POSITION=2, SHAKE_FOVY=4, SHAKE_BANK=8,
 SHAKE_BLURE_RATE=0x10, SHAKE_BLURE_MATRIX=0x20
};
// HD: dCamSetup_c fields shift by eight bytes within the expanded setup object.
struct CameraUtilSetupHD {
 /* 0x000 */ u8 mPadding0[0x78];
 /* 0x078 */ be<f32> mForceLockOffDist;
 /* 0x07C */ be<s32> mForceLockOffTimer;
 /* 0x080 */ u8 mPadding80[0xA8-0x80];
 /* 0x0A8 */ be<f32> mManualEndVal;
 /* 0x0AC */ u8 mPaddingAC[0x168-0xAC];
 f32 ForceLockOffDist() { return mForceLockOffDist; }
 s32 ForceLockOffTimer() { return mForceLockOffTimer; }
 f32 ManualEndVal() { return mManualEndVal; }
};
WWHD_SIZE(CameraUtilSetupHD, 0x168);
WWHD_OFFSET(CameraUtilSetupHD, mForceLockOffDist, 0x78);
WWHD_OFFSET(CameraUtilSetupHD, mForceLockOffTimer, 0x7C);
inline CameraUtilSetupHD* cameraUtilSetup(dCamera_c* camera) { return reinterpret_cast<CameraUtilSetupHD*>(camera->mCamSetup); }
WWHD_OFFSET(CameraUtilSetupHD, mManualEndVal, 0xA8);
struct CameraUtilAttentionHD { u8 mPadding0[0x20]; be<u32> mFlags; };
WWHD_OFFSET(CameraUtilAttentionHD, mFlags, 0x20);
// HD: the shake pattern reads five bytes starting at camera+0x54D.
struct CameraUtilShakePatternHD { be<u8> m548[5]; };
WWHD_SIZE(CameraUtilShakePatternHD, 5);
struct CameraUtilPointWordsHD { be<u32> x,y,z; };
WWHD_SIZE(CameraUtilPointWordsHD, 12);
struct CameraUtilMatrixHD { be<f32> m[3][4]; };
WWHD_SIZE(CameraUtilMatrixHD, 0x30);
struct CameraUtilGraphicsHD { be<u8> mBlure,mBlureRate; };
WWHD_SIZE(CameraUtilGraphicsHD, 2);
f32 cameraUtilCPad_GET_STICK_POS_X(s32);
f32 cameraUtilCPad_GET_STICK_POS_Y(s32);
f32 cameraUtilCPad_GET_STICK_VALUE(s32);
f32 cameraUtilCPad_GET_SUBSTICK_POS_X(s32);
f32 cameraUtilCPad_GET_SUBSTICK_POS_Y(s32);
f32 cameraUtilCPad_GET_SUBSTICK_VALUE(s32);
f32 cameraUtilCPad_GET_ANALOG_L(s32);
f32 cameraUtilCPad_GET_ANALOG_R(s32);
u32 cameraUtilmDoCPd_L_LOCK_BUTTON(s32);
u32 cameraUtilmDoCPd_L_LOCK_TRIGGER(s32);
u32 cameraUtilmDoCPd_R_LOCK_BUTTON(s32);
u32 cameraUtilmDoCPd_R_LOCK_TRIGGER(s32);
u32 cameraUtilCPad_CHECK_HOLD_X(s32);
u32 cameraUtilCPad_CHECK_TRIG_X(s32);
u32 cameraUtilCPad_CHECK_HOLD_Y(s32);
u32 cameraUtilCPad_CHECK_TRIG_Y(s32);
u32 cameraUtilCPad_CHECK_HOLD_Z(s32);
u32 cameraUtilCPad_CHECK_TRIG_Z(s32);
u32 cameraUtilCPad_CHECK_HOLD_B(s32);
u32 cameraUtilCPad_CHECK_TRIG_B(s32);
void cameraUtilMoveBgMatrixCrrPos(dBgS*,cBgS_PolyInfo*,BOOL,cXyz*,csXyz*,csXyz*);
f32 cameraUtilHorizontalDistance(const cXyz*,const cXyz*);
u32 cameraUtilButtonHold(s32);
cSAngle_l* cameraUtilAngleShort(cSAngle_l*,s32);
bool cameraUtilLockonTruth(dAttention_c*);
void cameraUtilEffectLineInit(void*,s32,s32,s32);
void cameraUtilEffectLineUpdate(void*,const cXyz*,const GXColor*,u32,u32,u32,u32,f32,f32,f32,f32);
void cameraUtilMatrixTrans(CameraUtilMatrixHD*,f32,f32,f32);
void cameraUtilMatrixXrot(CameraUtilMatrixHD*,s32);
void cameraUtilMatrixYrot(CameraUtilMatrixHD*,s32);
void cameraUtilMatrixZrot(CameraUtilMatrixHD*,s32);
void cameraUtilOnBlureMatrix(CameraUtilMatrixHD*);
cSAngle_l* cameraUtilAngleDegrees(cSAngle_l*,f32);
void cameraUtilOnBlure();
void cameraUtilProject(const cXyz*,cXyz*);
void cameraUtilVectorSubtract(const cXyz*,const cXyz*,cXyz*);
void cameraUtilAngleMultiply(const cSAngle_l*,cSAngle_l*,f32);
void cameraUtilAngleSubtractAssign(cSAngle_l*,const cSAngle_l*);
