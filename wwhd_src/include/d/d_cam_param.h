/** Camera parameters and math; GameCube class/field names, measured HD layouts.
 * HD adds explicit vtables, setup fields and result pointers; raw entry return widths remain
 * intact. */
#pragma once
#include "SSystem/SComponent/c_angle.h"
#include "gc_api/gc_api.h"

enum { dCamStyle_NN00_e = 0 };
enum dCamStyleParam_e {
    /* 0x00 */ dCamStyleParam_UNK0 = 0,
    /* 0x01 */ dCamStyleParam_UNK1 = 1,
    /* 0x02 */ dCamStyleParam_UNK2 = 2,
    /* 0x03 */ dCamStyleParam_UNK3 = 3,
    /* 0x04 */ dCamStyleParam_UNK4 = 4,
    /* 0x05 */ dCamStyleParam_CENTER_HEIGHT_BASE = 5,
    /* 0x06 */ dCamStyleParam_CENTER_HEIGHT_UPPER = 6,
    /* 0x07 */ dCamStyleParam_CENTER_HEIGHT_LOWER = 7,
    /* 0x08 */ dCamStyleParam_LOCKON_CENTER_HEIGHT_MIN = 8,
    /* 0x09 */ dCamStyleParam_LOCKON_CENTER_HEIGHT_MAX = 9,
    /* 0x0A */ dCamStyleParam_UNK10 = 10,
    /* 0x0B */ dCamStyleParam_UNK11 = 11,
    /* 0x0C */ dCamStyleParam_UNK12 = 12,
    /* 0x0D */ dCamStyleParam_UNK13 = 13,
    /* 0x0E */ dCamStyleParam_UNK14 = 14,
    /* 0x0F */ dCamStyleParam_UNK15 = 15,
    /* 0x10 */ dCamStyleParam_UNK16 = 16,
    /* 0x11 */ dCamStyleParam_UNK17 = 17,
    /* 0x12 */ dCamStyleParam_LOCKON_LATITUDE_MIN = 18,
    /* 0x13 */ dCamStyleParam_LOCKON_LATITUDE_MAX = 19,
    /* 0x14 */ dCamStyleParam_UNK20 = 20,
    /* 0x15 */ dCamStyleParam_UNK21 = 21,
    /* 0x16 */ dCamStyleParam_UNK22 = 22,
    /* 0x17 */ dCamStyleParam_LOCKON_LONGITUDE_MIN = 23,
    /* 0x18 */ dCamStyleParam_LOCKON_LONGITUDE_MAX = 24,
    /* 0x19 */ dCamStyleParam_FOVY_BASE = 25,
    /* 0x1A */ dCamStyleParam_FOVY_UPPER = 26,
    /* 0x1B */ dCamStyleParam_FOVY_LOWER = 27,
    /* 0x1C */ dCamStyleParam_LOCKON_FOVY_MIN = 28,
    /* 0x1D */ dCamStyleParam_LOCKON_FOVY_MAX = 29,
};
struct dCamera__Style {
    be<u32> m00;
    be<s32> engineIdx;
    be<f32> styleParam[30];
    be<u16> flag;
    u8 _82[2];
};
struct CamParamStyles {
    dCamera__Style mEntries[145];
};
struct dCstick_c {
    be<u32> __vtbl;
    be<f32> m00, m04;
    be<s32> m08;
    be<u16> mHDFlags;
    u8 _12[2];
    s32 Shift(u32 flags);
};
struct dCamBGChk_c {
    struct ChkInfo {
        be<f32> mDistance, mChkAngle, mWeightH, mWeightL;
    };
    be<u32> __vtbl;
    be<f32> mFloorMargin;
    ChkInfo mChkInfo[2];
    /* 0x28 */ be<f32> mFwdBackMargin;
    /* 0x2C */ be<f32> mFwdCushion;
    /* 0x30 */ be<f32> m2C;
    /* 0x34 */ be<f32> mGazeBackMargin;
    /* 0x38 */ be<f32> mCornerCushion;
    /* 0x3C */ be<f32> mWallCushion;
    /* 0x40 */ be<f32> mWallUpDistance;
    /* 0x44 */ be<f32> mWallBackCushion;
    /* 0x48 */ be<f32> mCornerAngleMax;
    /* 0x4C */ be<f32> m48;
    /* 0x50 */ be<f32> m4C;
    /* 0x54 */ be<f32> m50;
    /* 0x58 */ be<f32> m54;
    /* 0x5C */ be<f32> m58;
    /* 0x60 */ be<f32> m5C;
};
struct dCamSetup_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<f32> mDrawNear;
    /* 0x008 */ be<f32> mDrawFar;
    /* 0x00C */ be<u16> mDebugFlags;
    /* 0x00E */ be<u16> mFlags2;
    /* 0x010 */ be<s32> m00C;
    /* 0x014 */ be<s32> mModeSwitchType;
    /* 0x018 */ gptr<void> mTypeTable;
    /* 0x01C */ be<s32> mTypeNum;
    /* 0x020 */ be<s32> mForceType;
    /* 0x024 */ be<f32> mCusCus;
    /* 0x028 */ be<f32> m024;
    /* 0x02C */ be<f32> m028;
    /* 0x030 */ be<f32> m02C;
    /* 0x034 */ be<f32> m030;
    /* 0x038 */ be<f32> m034;
    /* 0x03C */ be<f32> mBaseCushion;
    /* 0x040 */ be<f32> mJumpCushion;
    /* 0x044 */ be<f32> mParallelDist;
    /* 0x048 */ be<f32> m044;
    /* 0x04C */ be<f32> mSubjLinkCullDist;
    /* 0x050 */ be<f32> mCurveWeight;
    /* 0x054 */ be<f32> m050;
    /* 0x058 */ be<f32> m054;
    /* 0x05C */ be<f32> m058;
    /* 0x060 */ be<f32> m05C;
    /* 0x064 */ be<f32> m060;
    /* 0x068 */ be<f32> mTrimVistaHeight;
    /* 0x06C */ be<f32> mTrimCineScopeHeight;
    /* 0x070 */ be<f32> m06C;
    /* 0x074 */ be<f32> mHDExtraParameter;
    /* 0x078 */ be<f32> mForceLockOffDist;
    /* 0x07C */ be<s32> mForceLockOffTimer;
    /* 0x080 */ be<f32> m078;
    /* 0x084 */ be<f32> m07C;
    /* 0x088 */ be<f32> m080;
    /* 0x08C */ be<f32> m084;
    /* 0x090 */ be<f32> mDMCValue;
    /* 0x094 */ be<f32> mDMCAngle;
    /* 0x098 */ be<f32> m090;
    /* 0x09C */ be<s32> m094;
    /* 0x0A0 */ be<f32> m098;
    /* 0x0A4 */ be<f32> mManualStartCThreshold;
    /* 0x0A8 */ be<f32> mManualEndVal;
    /* 0x0AC */ be<f32> mChargeLatitude;
    /* 0x0B0 */ be<s32> mChargeTimer;
    /* 0x0B4 */ be<f32> mChargeBRatio;
    /* 0x0B8 */ be<s32> m0B0;
    /* 0x0BC */ be<f32> m0B4;
    /* 0x0C0 */ be<f32> m0B8;
    /* 0x0C4 */ be<f32> m0BC;
    /* 0x0C8 */ be<f32> m0C0;
    /* 0x0CC */ be<f32> m0C4;
    /* 0x0D0 */ be<s32> m0C8;
    /* 0x0D4 */ cXyz mHDVector;
    /* 0x0E0 */ be<f32> mHDDefaults[4];
    /* 0x0F0 */ dCstick_c mCstick;
    /* 0x104 */ dCamBGChk_c mBGChk;
    BOOL CheckLatitudeRange(s16* angle);
    f32 FanBank();
};
struct dCamParam_c {
    be<u32> __vtbl;
    gptr<const dCamera__Style> mpStyle;
    be<s32> mStyleIdx;
    static const dCamera__Style (&styles)[145];
    static const be<s32>& style_num;
    BOOL Change(s32 index);
    s32 SearchStyle(u32 name);
    f32 ratiof(f32 t, f32 upper, f32 lower, f32 base);
    BOOL DefaultRadius(f32* radius);
    f32 RadiusRatio(f32 radius);
    f32 CenterHeight(f32 t);
    f32 Fovy(f32 t);
    s32 LockonLongitude(f32 t);
    s32 LockonLatitude(f32 t);
    f32 LockonFovy(f32 t);
    f32 LockonCenterHeight(f32 t);
};
struct dCamMath {
    static f32 rationalBezierRatio(f32 input, f32 weight);
    static f32 customRBRatio(f32 x, f32 y);
    static f32 zoomFovy(f32 degrees, f32 scale);
    // HD: explicit result and address-passed angle preserve the native ABI.
    static cXyz* xyzRotateX(cXyz* out, cXyz* vector, cSAngle* angle);
    static cXyz* xyzRotateY(cXyz* out, cXyz* vector, cSAngle* angle);
    static f32 xyzHorizontalDistance(cXyz* a, cXyz* b);
    static cXyz* xyzProjPosOnYZ(cXyz* out, cSAngle* angle, cXyz* a, cXyz* b);
};
struct CamParamAngles {
    cSAngle base, multiplied, difference, maximum;
};
WWHD_SIZE(dCamera__Style, 0x84);
WWHD_SIZE(dCstick_c, 0x14);
WWHD_SIZE(dCamBGChk_c, 0x64);
WWHD_SIZE(dCamSetup_c, 0x168);
WWHD_SIZE(dCamParam_c, 0xC);
WWHD_SIZE(CamParamAngles, 8);
WWHD_OFFSET(dCamSetup_c, mCstick, 0xF0);
WWHD_OFFSET(dCamSetup_c, mBGChk, 0x104);
WWHD_OFFSET(dCamParam_c, mpStyle, 4);
WWHD_OFFSET(dCamParam_c, mStyleIdx, 8);
dCstick_c* dCstick_c__ct(dCstick_c* self);
dCamParam_c* dCamParam_c__ct(dCamParam_c* self, s32 index);
void dCamParam_c__dt(dCamParam_c* self, s32 flag);
dCamBGChk_c* dCamBGChk_c__ct(dCamBGChk_c* self);
dCamSetup_c* dCamSetup_c__ct(dCamSetup_c* self);
void dCamSetup_c__dt(dCamSetup_c* self, s32 flag);

enum : u32 {
    CAM_PARAM_STICK_VTABLE = 0x10044708,
    CAM_PARAM_BG_VTABLE = 0x10044720,
    CAM_PARAM_STYLE_VTABLE = 0x10044840,
    CAM_PARAM_SETUP_VTABLE = 0x10044858
};
struct CamParamHeaderStatics {
    be<f32> mNegativePi, mPi;
    u8 mFirstObject, mSecondObject;
    u8 _0A[2];
    be<u32> mZeroWords[4];
};
struct CamParamDestructorRecord {
    u8 mStorage[12];
};
struct CamParamDestructorRecords {
    CamParamDestructorRecord mEntries[3];
};
WWHD_SIZE(CamParamHeaderStatics, 0x1C);
WWHD_SIZE(CamParamDestructorRecords, 0x24);
u32 CamParam_allocate(u32 size);
void CamParam_deallocate(void* memory);
void CamParam_angleDegree(cSAngle* angle, f32 degrees);
void CamParam_subtractAngles(cSAngle* left, cSAngle* result, cSAngle* right);
void CamParam_multiplyAngle(cSAngle* angle, cSAngle* result, f32 scale);
void CamParam_addAngle(cSAngle* angle, cSAngle* right);
void CamParam_degreeConstruct(cDegree* degree, f32 value);
f32 CamParam_degreeCos(cDegree* degree);
f32 CamParam_degreeSin(cDegree* degree);
f32 CamParam_atan2(f32 y, f32 x);
void CamParam_XrotS(Mtx34* matrix, s32 angle);
void CamParam_YrotS(Mtx34* matrix, s32 angle);
void CamParam_subtractVectors(cXyz* left, cXyz* result, cXyz* right);
void CamParam_negateAngle(cSAngle* angle, cSAngle* result);
void CamParam_copyAngle(cSAngle* angle, cSAngle* source);
cXyz* CamParam_addVectors(cXyz* left, cXyz* result, cXyz* right);
void CamParam_registerStatic(void* descriptor);
void CamParam_initFirstObject(u8* object);
void CamParam_initSecondObject(u8* object);

enum : u32 { CAM_PARAM_DELETE_STORAGE = 1 };

WWHD_OFFSET(dCstick_c, m04, 8);
WWHD_OFFSET(dCstick_c, m08, 0xC);
WWHD_OFFSET(dCamBGChk_c, mChkInfo, 8);
WWHD_OFFSET(dCamBGChk_c, mFwdCushion, 0x2C);
WWHD_OFFSET(dCamSetup_c, m05C, 0x60);
WWHD_OFFSET(dCamSetup_c, m060, 0x64);
WWHD_OFFSET(dCamSetup_c, m080, 0x88);
WWHD_OFFSET(dCamSetup_c, m084, 0x8C);
WWHD_OFFSET(dCamSetup_c, mHDVector, 0xD4);
WWHD_OFFSET(CamParamHeaderStatics, mZeroWords, 0xC);
