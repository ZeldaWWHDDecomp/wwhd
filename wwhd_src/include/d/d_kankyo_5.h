// Color, fog, sound and time helpers. HD shader-light objects replace GX register uploads.
#pragma once
#include "d/d_kankyo.h"
namespace d_kankyo_cpp {
enum ColorChangeState {
    COLOR_CHANGE_NONE_e = 0,
    COLOR_CHANGE_ITEM_INIT_e = 1,
    COLOR_CHANGE_ITEM_WAIT_e = 4,
    COLOR_CHANGE_ITEM_END_e = 5,
    COLOR_CHANGE_ITEM_REINIT_e = 6,
    COLOR_CHANGE_ARROW_DEFAULT_e = 0x10,
    COLOR_CHANGE_ARROW_MODE1_e = 0x20,
    COLOR_CHANGE_ARROW_MODE2_e = 0x30
};
struct ColorFogColors {
    GXColor mActorC0, mActorK0, mBG0_C0, mBG0_K0, mBG1_C0, mBG1_K0, mBG2_C0, mBG2_K0, mBG3_C0, mBG3_K0;
    GXColor mFogColor, mVrboxSkyCol, mVrUsoUmiColor;
    u8 reserved[16];
    GXColorS10 mActorDifColor;
};
WWHD_SIZE(ColorFogColors, 0x4C);
WWHD_OFFSET(ColorFogColors, mFogColor, 0x28);
WWHD_OFFSET(ColorFogColors, mVrUsoUmiColor, 0x30);
WWHD_OFFSET(ColorFogColors, mActorDifColor, 0x44);
struct ColorFogRoomLight {
    be<f32> mParameters[6];
    be<u8> mFlags[4];
};
WWHD_SIZE(ColorFogRoomLight, 0x1C);
struct ColorFogEnvironment : KankyoEnvLight {
    f32 getDaytime();
    be<f32> *colorRatios() { return &mAllColRatio; }
    be<f32> *shaderLightRatios() { return &field_0x10c0; }
    be<f32> *shaderLightFactors() { return &field_0x10d4; }
    ColorFogRoomLight &roomLighting() { return *reinterpret_cast<ColorFogRoomLight *>(mHdTailState); }
    be<u16> *lightPitchTable() { return reinterpret_cast<be<u16> *>(mHdTailTable0); }
    be<u16> *lightYawTable() { return reinterpret_cast<be<u16> *>(mHdTailTable1); }
    ColorFogColors &colors() { return *reinterpret_cast<ColorFogColors *>(&mHdField_0xb64); }
};
WWHD_SIZE(ColorFogEnvironment, 0x120C);
inline be<u32> &colorFogColorWord(GXColor &c) { return *reinterpret_cast<be<u32> *>(&c); }
inline be<u8> *colorFogChannels(GXColor &c) { return reinterpret_cast<be<u8> *>(&c); }
inline be<s16> *colorFogChannels(GXColorS10 &c) { return reinterpret_cast<be<s16> *>(&c); }
struct ColorFogCamera {
    u8 prefix[0xDC];
    cXyz mEye, mCenter;
};
WWHD_SIZE(ColorFogCamera, 0xF4);
WWHD_OFFSET(ColorFogCamera, mEye, 0xDC);
struct ColorFogView {
    u8 prefix[0xCC];
    be<f32> mNear, mFar, mFovy;
};
WWHD_SIZE(ColorFogView, 0xD8);
WWHD_OFFSET(ColorFogView, mNear, 0xCC);

struct ColorFogStringVtable {
    u8 prefix[0x14];
    be<u32> assureTermination;
};
WWHD_SIZE(ColorFogStringVtable, 0x18);
WWHD_OFFSET(ColorFogStringVtable, assureTermination, 0x14);
struct ColorFogStageVtable {
    u8 prefix[0x9C];
    be<u32> getPalette;
    u8 gap0[0xC];
    be<u32> getPselect;
    u8 gap1[0xC];
    be<u32> getEnvr;
    u8 gap2[0xC];
    be<u32> getVrbox;
    u8 gap3[0x15C - 0xD0];
    be<u32> getStagInfo;
};
WWHD_SIZE(ColorFogStageVtable, 0x160);
WWHD_OFFSET(ColorFogStageVtable, getStagInfo, 0x15C);
struct ColorFogStageObject {
    be<u32> __vtbl;
};
WWHD_SIZE(ColorFogStageObject, 4);
struct ColorFogStageInfo {
    u8 prefix[0xC];
    be<u32> mParameters;
};
WWHD_SIZE(ColorFogStageInfo, 0x10);
WWHD_OFFSET(ColorFogStageInfo, mParameters, 0xC);

struct ColorFogPlay {
    u8 prefix[0x12A0];
    u8 mBgS[0x5134 - 0x12A0];
    char mStageName[8];
    u8 stageGap[0x5150 - 0x513C];
    ColorFogStageObject mStage;
    u8 cameraGap[0x5AF8 - 0x5154];
    gptr<ColorFogCamera> mpCamera;
    u8 reserved[0x5B2C - 0x5AFC];
    gptr<fopAc_ac_c> mpPlayer;
    u8 rest[0x5BB3 - 0x5B30];
    be<u8> mScopeMesgStatus;
    u8 tail[0x5FA4 - 0x5BB4];
    gptr<ColorFogView> mpView;
};
WWHD_SIZE(ColorFogPlay, 0x5FA8);
WWHD_OFFSET(ColorFogPlay, mBgS, 0x12A0);
WWHD_OFFSET(ColorFogPlay, mStageName, 0x5134);
WWHD_OFFSET(ColorFogPlay, mStage, 0x5150);
WWHD_OFFSET(ColorFogPlay, mpCamera, 0x5AF8);
WWHD_OFFSET(ColorFogPlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(ColorFogPlay, mScopeMesgStatus, 0x5BB3);
WWHD_OFFSET(ColorFogPlay, mpView, 0x5FA4);
struct ColorFogSave {
    u8 prefix[0x44];
    be<f32> mTime;
    be<u16> mDate;
    u8 rest[0x71 - 0x4A];
    u8 mItems[1];
};
WWHD_SIZE(ColorFogSave, 0x72);
WWHD_OFFSET(ColorFogSave, mTime, 0x44);
struct ColorFogSavePointer {
    gptr<ColorFogSave> value;
};
WWHD_SIZE(ColorFogSavePointer, 4);
struct ColorFogString {
    gptr<const char> str;
    be<u32> vtbl;
};
WWHD_SIZE(ColorFogString, 8);
// HD: a parameter resource groups material slots and polymorphic shader objects.
struct LightParameterEntry {
    be<u16> mFirstObject, mObjectCount;
};
WWHD_SIZE(LightParameterEntry, 4);
struct LightParameterVtable {
    u8 reserved[0x44];
    be<u32> checkDerivedRuntimeTypeInfo;
};
WWHD_SIZE(LightParameterVtable, 0x48);
WWHD_OFFSET(LightParameterVtable, checkDerivedRuntimeTypeInfo, 0x44);
struct LightParameterObject {
    u8 prefix[0x58];
    be<u32> __vtbl;
    u8 reserved[0xF4 - 0x5C];
    union {
        be<f32> mLightColor[4];
        be<f32> mFogStart;
    };
    be<f32> mFogEnd;
    u8 rest[0x114 - 0x108];
    be<f32> mFogColor[4];
    u8 tail[8];
    cXyz mLightPosition;
};
WWHD_SIZE(LightParameterObject, 0x138);
WWHD_OFFSET(LightParameterObject, __vtbl, 0x58);
WWHD_OFFSET(LightParameterObject, mLightColor, 0xF4);
WWHD_OFFSET(LightParameterObject, mFogStart, 0xF4);
WWHD_OFFSET(LightParameterObject, mFogEnd, 0x104);
WWHD_OFFSET(LightParameterObject, mFogColor, 0x114);
WWHD_OFFSET(LightParameterObject, mLightPosition, 0x12C);
struct LightParameterTable {
    u8 prefix[8];
    be<u32> mEntryCount;
    gptr<LightParameterEntry> mpEntries;
    be<u32> mObjectCount;
    gptr<gptr<LightParameterObject>> mpObjects;
};
WWHD_SIZE(LightParameterTable, 0x18);
WWHD_OFFSET(LightParameterTable, mpEntries, 0xC);
struct LightParameterTablePointer {
    gptr<LightParameterTable> value;
};
WWHD_SIZE(LightParameterTablePointer, 4);
struct LightParameterIndex {
    gptr<be<s16>> value;
};
WWHD_SIZE(LightParameterIndex, 4);

// HD: each copied shader light has the original light-object stride.
struct ColorFogLightObject {
    cXyz mLightPosition, mLightDirection;
    GXColor mColor;
    GXColorS10 mColorS10;
    cXyz mCosAtten, mDistAtten;
    be<f32> mRefDistance, mRefBrightness;
    u8 reserved[0x84 - 0x44];
};
WWHD_SIZE(ColorFogLightObject, 0x84);
WWHD_OFFSET(ColorFogLightObject, mColor, 0x18);
WWHD_OFFSET(ColorFogLightObject, mColorS10, 0x1C);
WWHD_OFFSET(ColorFogLightObject, mCosAtten, 0x24);
WWHD_OFFSET(ColorFogLightObject, mRefBrightness, 0x40);
struct ColorFogTevStr {
    /* 0x00 */ ColorFogLightObject mLightObj;
    /* 0x84 */ cXyz mLightPosWorld; /* GameCube 0x74 */
    /* 0x90 */ GXColorS10 mColorC0; /* GameCube 0x80 */
    /* 0x98 */ GXColor mColorK0;
    /* 0x9C */ GXColor mColorK1;
    /* 0xA0 */ GXColorS10 mFogColor;
    /* 0xA8 */ be<f32> mFogStartZ;
    /* 0xAC */ be<f32> mFogEndZ;
    /* 0xB0 */ be<f32> mColpatBlend;
    /* 0xB4 */ be<u8> mInitTimer;
    /* 0xB5 */ be<u8> mEnvrIdxCurr;
    /* 0xB6 */ be<u8> mEnvrIdxPrev;
    /* 0xB7 */ be<u8> mColpatCurr;
    /* 0xB8 */ be<u8> mColpatPrev;
    /* 0xB9 */ be<s8> mRoomNo; /* GameCube 0xA9 */
    /* 0xBA */ be<u8> mEnvrIdxOverride;
    /* 0xBB */ be<u8> mLightMode;
    /* 0xBC */ be<u8> mInitType; /* 123 after dKy_tevstr_init, 124 once set */
    /* 0xBD */ u8 _BD[3];
    /* 0xC0 */ ColorFogLightObject mHdLightParam[2];
};
WWHD_SIZE(ColorFogTevStr, 0x1C8);
WWHD_OFFSET(ColorFogTevStr, mFogColor, 0xA0);
WWHD_OFFSET(ColorFogTevStr, mInitType, 0xBC);
WWHD_OFFSET(ColorFogTevStr, mHdLightParam, 0xC0);

struct ColorFogRGBA {
    be<f32> r, g, b, a;
};
WWHD_SIZE(ColorFogRGBA, 0x10);
struct ColorFogMatrix {
    be<f32> values[12];
};
WWHD_SIZE(ColorFogMatrix, 0x30);
struct ColorFogJ3DSystem {
    u8 prefix[0x38];
    ColorFogMatrix mViewMtx;
    u8 reserved[0x148 - 0x68];
    gptr<LightParameterTable> mpParameters;
};
WWHD_SIZE(ColorFogJ3DSystem, 0x14C);
WWHD_OFFSET(ColorFogJ3DSystem, mViewMtx, 0x38);
WWHD_OFFSET(ColorFogJ3DSystem, mpParameters, 0x148);
struct ColorFogVoidPointer {
    gptr<void> value;
};
WWHD_SIZE(ColorFogVoidPointer, 4);

struct ColorFogGndCheck {
    enum { FULL_GRP = 0xF };
    be<u32> mPolyPassChk, mGrpPassChk;
    be<u32> mActorPid;
    be<u8> mSameActorChk;
    u8 pad[3];
    be<u32> mCheckVtable;
    u8 polyInfo[0x20 - 0x14];
    be<u32> mPolyInfoVtable;
    cXyz m_pos;
    be<u32> mFlag;
    be<f32> mNowY;
    be<u32> mWallPrecheck, mGndPrecheck;
    be<u32> mPolyPassVtable;
    be<u8> mbObjThrough, mbCamThrough, mbLinkThrough, mbArrowThrough, mbBombThrough, mbBoomerangThrough, mbRopeThrough;
    u8 padPass;
    be<u32> mGroupPassVtable, mGrp;
};
WWHD_SIZE(ColorFogGndCheck, 0x54);
WWHD_OFFSET(ColorFogGndCheck, mCheckVtable, 0x10);
WWHD_OFFSET(ColorFogGndCheck, mPolyInfoVtable, 0x20);
WWHD_OFFSET(ColorFogGndCheck, m_pos, 0x24);
WWHD_OFFSET(ColorFogGndCheck, mPolyPassVtable, 0x40);
WWHD_OFFSET(ColorFogGndCheck, mbObjThrough, 0x44);
WWHD_OFFSET(ColorFogGndCheck, mGroupPassVtable, 0x4C);
WWHD_OFFSET(ColorFogGndCheck, mGrp, 0x50);

struct ColorFogFloatRegisters {
    be<f32> values[20];
};
WWHD_SIZE(ColorFogFloatRegisters, 80);
struct ColorFogShortRegisters {
    be<u16> values[10];
};
WWHD_SIZE(ColorFogShortRegisters, 20);
struct ColorFogWord {
    be<u32> value;
};
WWHD_SIZE(ColorFogWord, 4);
// GHS word copies preserve signaling NaNs in float fields.
inline be<u32> &colorFogWordBits(be<f32> &v) { return *reinterpret_cast<be<u32> *>(&v); }
struct ColorFogFloat {
    be<f32> value;
};
WWHD_SIZE(ColorFogFloat, 4);
// HD: status records retain the expanded stride used by the light manager.
struct ColorFogLightStatus {
    cXyz mPos, mPos2;
    GXColor mColor;
    u8 reserved[0x38 - 0x1C];
    cXyz mLightDir;
    u8 tail[0xE8 - 0x44];
};
WWHD_SIZE(ColorFogLightStatus, 0xE8);
WWHD_OFFSET(ColorFogLightStatus, mColor, 0x18);
WWHD_OFFSET(ColorFogLightStatus, mLightDir, 0x38);
struct ColorFogLightStatuses {
    ColorFogLightStatus values[8];
};
WWHD_SIZE(ColorFogLightStatuses, 0x740);
struct ColorFogLightStatusPointer {
    gptr<ColorFogLightStatus> value;
};
WWHD_SIZE(ColorFogLightStatusPointer, 4);
struct ColorFogLightMask {
    be<u16> value;
};
WWHD_SIZE(ColorFogLightMask, 2);
struct ColorFogLightMasks {
    be<u16> values[8];
};
WWHD_SIZE(ColorFogLightMasks, 16);
} // namespace d_kankyo_cpp
d_kankyo_cpp::ColorFogEnvironment *colorFogEnv();
d_kankyo_cpp::ColorFogPlay *colorFogPlay();
f32 colorFogSquareDistance(const cXyz *, const cXyz *);
f32 colorFogSqrt(f32);
void colorFogRainSet(s32);

void colorFogProject(cXyz *, cXyz *);
BOOL colorFogMoonArrivalCheck();
void *colorFogAllocate(u32);

void colorFogClearWords(d_kankyo_cpp::ColorFogTevStr *, u32);

void colorFogGetVector(cXyz *, cXyz *, cXyz *);
f32 colorFogAddCalc(be<f32> *, f32, f32, f32, f32);
f32 colorFogAddCalc2(be<f32> *, f32, f32, f32);
f32 colorFogRandom(f32);
void colorFogScaleColor(d_kankyo_cpp::ColorFogRGBA *, f32);
void colorFogSubmitLight(void *, cXyz *, d_kankyo_cpp::ColorFogRGBA *, s32, void *, s32, s32, s32, f32, f32, f32);
void colorFogInverseTranspose(d_kankyo_cpp::ColorFogMatrix *, d_kankyo_cpp::ColorFogMatrix *);
void colorFogMatrixVector(d_kankyo_cpp::ColorFogMatrix *, cXyz *, cXyz *);
void colorFogConvertColor(d_kankyo_cpp::ColorFogRGBA *, GXColor *);
void colorFogConvertColorS10(d_kankyo_cpp::ColorFogRGBA *, GXColorS10 *);

void colorFogConstructGround(d_kankyo_cpp::ColorFogGndCheck *);
f32 colorFogGroundCross(void *, d_kankyo_cpp::ColorFogGndCheck *);
void colorFogDestroyCheck(d_kankyo_cpp::ColorFogGndCheck *, s32);
s32 colorFogRainCheck();

void colorFogInitWindowPos();
void colorFogInitSeaEnvPos();
void colorFogInitRiverPos();
void colorFogXFogTableSet(u32);
void *colorFogBossSchedule();
void *colorFogSchedule();
void *colorFogDefaultEnvr();
void *colorFogDefaultPalette();
void *colorFogDefaultPselect();
void *colorFogDefaultVrbox();
BOOL colorFogIsGetItem(void *, u32, u32);
void colorFogMemset(void *, s32, u32);
void colorFogMemcpy(void *, const void *, u32);
void colorFogWaveInit();
void colorFogWindSet();
void *colorFogObjectResource(const char *, const char *);
void colorFogSoundStart(u32);
void colorFogMuteMtDragon();
void colorFogSkyCloisters();

namespace d_kankyo_cpp {
WWHD_OFFSET(ColorFogColors, mActorK0, 4);
WWHD_OFFSET(ColorFogColors, mBG1_C0, 0x10);
WWHD_OFFSET(ColorFogColors, mBG1_K0, 0x14);
WWHD_OFFSET(ColorFogView, mFar, 0xD0);
WWHD_OFFSET(ColorFogView, mFovy, 0xD4);
WWHD_OFFSET(ColorFogSave, mDate, 0x48);
WWHD_OFFSET(ColorFogSave, mItems, 0x71);
WWHD_OFFSET(LightParameterTable, mEntryCount, 8);
WWHD_OFFSET(LightParameterTable, mObjectCount, 0x10);
WWHD_OFFSET(LightParameterTable, mpObjects, 0x14);
WWHD_OFFSET(ColorFogLightObject, mLightDirection, 0xC);
WWHD_OFFSET(ColorFogLightObject, mDistAtten, 0x30);
WWHD_OFFSET(ColorFogLightObject, mRefDistance, 0x3C);
WWHD_OFFSET(ColorFogRoomLight, mFlags, 0x18);
WWHD_OFFSET(ColorFogLightStatus, mPos2, 0xC);
} // namespace d_kankyo_cpp
