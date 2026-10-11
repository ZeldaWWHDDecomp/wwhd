// Camera accessors and HD adapters.
#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"

struct CameraProcessCoreHD {
    u8 _00[0x248];
    dCamera_c mCamera;
};
WWHD_OFFSET(CameraProcessCoreHD, mCamera, 0x248);
struct CameraRestoreCoreHD {
    u8 _00[0xD4];
    be<f32> mFovy;
    u8 _D8[4];
    struct {
        cXyz mEye, mCenter;
    } mLookat;
    u8 _F4[0xC];
    cSAngle_l mBank;
};
WWHD_OFFSET(CameraRestoreCoreHD, mLookat, 0xDC);
WWHD_OFFSET(CameraRestoreCoreHD, mBank, 0x100);
struct CameraWindowCoreHD {
    struct {
        be<f32> mX, mY, mWidth, mHeight, mNearZ, mFarZ;
        struct {
            be<f32> mX, mY, mWidth, mHeight;
        } mScissor;
    } mViewport;
    be<s8> mCameraID, mMode;
    u8 _2A[2];
};
WWHD_SIZE(CameraWindowCoreHD, 0x2C);
WWHD_OFFSET(CameraWindowCoreHD, mViewport.mNearZ, 0x10);
// HD: the five-byte patterns overlap the GC header's four-byte declarations differently.
struct CameraShakePatternCoreHD {
    u8 _00[0x548];
    be<u8> m544[5], m548[5];
};
WWHD_OFFSET(CameraShakePatternCoreHD, m548, 0x54D);
struct CameraGlobeWordsHD {
    be<u32> mRadius, mAngles;
};
WWHD_SIZE(CameraGlobeWordsHD, 8);
struct CameraQuaternionCoreHD {
    be<f32> x, y, z, w;
};
WWHD_SIZE(CameraQuaternionCoreHD, 0x10);
struct CameraMatrixCoreHD {
    be<f32> m[3][3];
};
WWHD_SIZE(CameraMatrixCoreHD, 0x24);
struct CameraHeaderStaticsCoreHD {
    be<f32> negativePi, positivePi;
    u8 firstObject, secondObject;
    u8 _0A[2];
};
WWHD_SIZE(CameraHeaderStaticsCoreHD, 0xC);
// Unit-owned adapters keep tww method names without modifying the shared class.
// HD: vector value parameters use the verified pointer ABI; ResetView takes a view.
// Separate adapters avoid ambiguous overload registration in autoglue.
class CameraTypeForceIdHD : public dCamera_c {
  public:
    bool SetTypeForce(s32 type, fopAc_ac_c* actor);
};
class CameraTypeForceNameHD : public dCamera_c {
  public:
    void SetTypeForce(const char* name, fopAc_ac_c* actor);
};
class CameraSetHD : public dCamera_c {
  public:
    bool Set(cXyz* center, cXyz* eye);
};
class CameraSetFovBankHD : public dCamera_c {
  public:
    bool Set(cXyz* center, cXyz* eye, f32 fovY, s16 bank);
};
class CameraSetBankFovHD : public dCamera_c {
  public:
    bool Set(cXyz* center, cXyz* eye, s16 bank, f32 fovY);
};
class CameraResetHD : public dCamera_c {
  public:
    bool Reset(cXyz* center, cXyz* eye);
};
class CameraResetFovBankHD : public dCamera_c {
  public:
    bool Reset(cXyz* center, cXyz* eye, f32 fovY, s16 bank);
};
class CameraBlureScaleHD : public dCamera_c {
  public:
    void SetBlureScale(f32 scale);
};
class CameraBlureScaleVectorHD : public dCamera_c {
  public:
    void SetBlureScale(f32 x, f32 y, f32 z);
};
class CameraAccessorsHD : public dCamera_c {
  public:
    void ResetView(CameraRestoreCoreHD* view);
    s32 StartShake(s32 length, u8* pattern, s32 flags, cXyz* pos);
};
WWHD_SIZE(CameraAccessorsHD, sizeof(dCamera_c));
cSAngle_l* camCore_angleFromShort(cSAngle_l*, s16);
cSAngle_l* camCore_angleCopy(cSAngle_l*, const cSAngle_l*);
f32 camCore_radiusActorInSight(dCamera_c*, fopAc_ac_c*, fopAc_ac_c*, cXyz*, cXyz*, f32, s16);
bool camCore_SetTypeForce(dCamera_c*, s32, fopAc_ac_c*);
void camCore_cXyzNorm(cXyz*, cXyz*);
s32 camCore_fopCamM_GetParam(camera_class*);
void camCore_setViewPort(CameraWindowCoreHD*, f32, f32, f32, f32, f32, f32);
void camCore_setScissor(CameraWindowCoreHD*, f32, f32, f32, f32);
f32 camCore_PSVECMag(cXyz*);
f32 camCore_cosf(f32);
f32 camCore_sinf(f32);

struct CameraHeaderDescriptorsCoreHD {
    u8 mDescriptors[0x24];
};
struct CameraHeaderZeroCoreHD {
    be<u32> mZero[4];
};
