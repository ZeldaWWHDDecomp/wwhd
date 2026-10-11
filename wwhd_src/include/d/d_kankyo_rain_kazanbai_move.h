// Weather motion and lens drawing. GameCube names with explicit HD particle/renderer layouts.
// HD: packet headers, SafeStrings and render buffers replace the GameCube texture/GX fields.
#pragma once
#include "bindings.h"
#include <bit>
namespace RainMotion {
using namespace gabi;
union Vector {
    cXyz value;
    be<f32> channels[3];
    be<u32> words[3];
};
struct DOUBLE_POS {
    be<f64> x, y, z;
};
struct Matrix {
    be<f32> cells[12];
};
union ColorF {
    be<f32> channels[4];
    be<u32> words[4];
};
union Color {
    be<u32> word;
    be<u8> channels[4];
};
union ProjectionParameters {
    be<u32> words[10];
    be<f32> values[10];
};
struct SafeString {
    be<u32> text, vtable;
};
struct SinCos {
    be<f32> sine, cosine;
};
WWHD_SIZE(Vector, 12);
WWHD_SIZE(DOUBLE_POS, 24);
WWHD_SIZE(Matrix, 48);
WWHD_SIZE(ColorF, 16);
WWHD_SIZE(Color, 4);
WWHD_SIZE(ProjectionParameters, 40);
WWHD_SIZE(SafeString, 8);
struct PolyPass {
    be<u32> vtable;
    be<u8> flags[7];
    u8 padding;
};
struct GroundCheck {
    cBgS_GndChk base;
    PolyPass pass;
    dBgS_GrpPassChk group;
};
WWHD_SIZE(GroundCheck, 84);
struct RendererBounds {
    u8 padding0[0x130];
    /* 0x130 */ be<f32> lower;
    /* 0x134 */ be<f32> upper;
};
WWHD_OFFSET(RendererBounds, lower, 0x130);
WWHD_OFFSET(RendererBounds, upper, 0x134);
enum : u32 { ProjectionDefaults = 0x1004F4F4, RendererAddress = 0x104B45C0 };
union Count32 {
    be<u32> word;
    struct {
        be<s16> upper, lower;
    } halves;
};
struct RAIN_EFF {
    /* 0x0 */ be<u8> mStatus;
    u8 padding1[0x3];
    /* 0x4 */ Vector mPos;
    /* 0x10 */ Vector mBasePos;
    /* 0x1C */ be<f32> field_0x1c;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<f32> mAlpha;
    /* 0x2C */ be<f32> field_0x2c;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<s16> mTimer;
    u8 tail[0x2];
};
WWHD_OFFSET(RAIN_EFF, mStatus, 0x0);
WWHD_OFFSET(RAIN_EFF, mPos, 0x4);
WWHD_OFFSET(RAIN_EFF, mBasePos, 0x10);
WWHD_OFFSET(RAIN_EFF, field_0x1c, 0x1C);
WWHD_OFFSET(RAIN_EFF, field_0x20, 0x20);
WWHD_OFFSET(RAIN_EFF, field_0x24, 0x24);
WWHD_OFFSET(RAIN_EFF, mAlpha, 0x28);
WWHD_OFFSET(RAIN_EFF, field_0x2c, 0x2C);
WWHD_OFFSET(RAIN_EFF, field_0x30, 0x30);
WWHD_OFFSET(RAIN_EFF, mTimer, 0x34);
WWHD_SIZE(RAIN_EFF, 0x38);
struct SNOW_EFF {
    /* 0x0 */ be<u8> mStatus;
    u8 padding1[0x3];
    /* 0x4 */ Vector mPos;
    /* 0x10 */ Vector mBasePos;
    /* 0x1C */ be<f32> mPosWaveX;
    /* 0x20 */ be<f32> mPosWaveZ;
    /* 0x24 */ be<f32> mGravity;
    /* 0x28 */ be<f32> mWindSpeed;
    /* 0x2C */ be<f32> mScale;
    u8 padding30[0x4];
    /* 0x34 */ be<s16> mTimer;
    u8 tail[0x2];
};
WWHD_OFFSET(SNOW_EFF, mStatus, 0x0);
WWHD_OFFSET(SNOW_EFF, mPos, 0x4);
WWHD_OFFSET(SNOW_EFF, mBasePos, 0x10);
WWHD_OFFSET(SNOW_EFF, mPosWaveX, 0x1C);
WWHD_OFFSET(SNOW_EFF, mPosWaveZ, 0x20);
WWHD_OFFSET(SNOW_EFF, mGravity, 0x24);
WWHD_OFFSET(SNOW_EFF, mWindSpeed, 0x28);
WWHD_OFFSET(SNOW_EFF, mScale, 0x2C);
WWHD_OFFSET(SNOW_EFF, mTimer, 0x34);
WWHD_SIZE(SNOW_EFF, 0x38);
struct RainPacket {
    u8 padding0[0xA0];
    /* 0xA0 */ RAIN_EFF mEff[250];
    /* 0x3750 */ be<f32> field_0x36c8;
    /* 0x3754 */ be<f32> field_0x36cc;
    /* 0x3758 */ be<s16> mRainCount;
    u8 padding375A[0x6];
    /* 0x3760 */ Vector mCamEyePos;
    /* 0x376C */ Vector mCamCenterPos;
    /* 0x3778 */ Vector mCenterDelta;
    /* 0x3784 */ be<f32> mCenterDeltaMul;
    u8 padding3788[0x8];
    /* 0x3790 */ be<f32> mOverheadFade;
    /* 0x3794 */ be<f32> mFwdFade1;
    /* 0x3798 */ be<f32> mFwdFade2;
    /* 0x379C */ be<u8> mStatus;
};
WWHD_OFFSET(RainPacket, mEff, 0xA0);
WWHD_OFFSET(RainPacket, field_0x36c8, 0x3750);
WWHD_OFFSET(RainPacket, field_0x36cc, 0x3754);
WWHD_OFFSET(RainPacket, mRainCount, 0x3758);
WWHD_OFFSET(RainPacket, mCamEyePos, 0x3760);
WWHD_OFFSET(RainPacket, mCamCenterPos, 0x376C);
WWHD_OFFSET(RainPacket, mCenterDelta, 0x3778);
WWHD_OFFSET(RainPacket, mCenterDeltaMul, 0x3784);
WWHD_OFFSET(RainPacket, mOverheadFade, 0x3790);
WWHD_OFFSET(RainPacket, mFwdFade1, 0x3794);
WWHD_OFFSET(RainPacket, mFwdFade2, 0x3798);
WWHD_OFFSET(RainPacket, mStatus, 0x379C);
struct SnowPacket {
    u8 padding0[0x98];
    /* 0x98 */ be<u32> mpTexture;
    /* 0x9C */ SNOW_EFF mEff[250];
    /* 0x374C */ Vector mOldEyePos;
    /* 0x3758 */ be<f32> field_0x36d0;
    /* 0x375C */ be<f32> field_0x36d4;
    /* 0x3760 */ be<s16> mEffCount;
    /* 0x3762 */ be<s16> mTamariStart;
};
WWHD_OFFSET(SnowPacket, mpTexture, 0x98);
WWHD_OFFSET(SnowPacket, mEff, 0x9C);
WWHD_OFFSET(SnowPacket, mOldEyePos, 0x374C);
WWHD_OFFSET(SnowPacket, field_0x36d0, 0x3758);
WWHD_OFFSET(SnowPacket, field_0x36d4, 0x375C);
WWHD_OFFSET(SnowPacket, mEffCount, 0x3760);
WWHD_OFFSET(SnowPacket, mTamariStart, 0x3762);
struct SunPacket {
    u8 padding0[0x98];
    /* 0x98 */ Vector mPos[2];
    u8 paddingB0[0x18];
    /* 0xC8 */ be<f32> mVisibility;
};
WWHD_OFFSET(SunPacket, mPos, 0x98);
WWHD_OFFSET(SunPacket, mVisibility, 0xC8);
union CacheLine {
    be<u32> words[8];
    be<u8> bytes[32];
};
struct BufferSlot {
    be<u32> buffer;
    u8 flushObject[0x14C];
    be<u32> size;
    u8 gap[4];
    u8 drawObject[0xFC];
};
struct Geometry {
    BufferSlot slots[2];
    be<u32> selected;
    u8 tail[20];
};
struct TriangleBank {
    BufferSlot slots[32];
    be<u32> selected;
};
struct FlareMaterial {
    u8 padding[0x168];
    ColorF mColor, mSecondaryColor;
    u8 tail[0x294];
};
struct FlareVertex {
    Vector mPos;
    u8 padding[0x2C];
    be<f32> mU, mV;
    u8 tail[0x58];
};
struct FlareBuffer {
    FlareVertex vertices[4];
};
WWHD_SIZE(BufferSlot, 0x254);
WWHD_SIZE(Geometry, 0x4C0);
WWHD_SIZE(FlareMaterial, 0x41C);
WWHD_SIZE(FlareVertex, 0x98);
WWHD_SIZE(FlareBuffer, 0x260);
WWHD_OFFSET(TriangleBank, selected, 0x4A80);
struct LensPacket {
    u8 padding0[0xA4];
    /* 0xA4 */ Vector mPositions[9];
    u8 padding110[0x8];
    /* 0x118 */ be<f32> field_0x88;
    /* 0x11C */ be<f32> field_0x8c;
    /* 0x120 */ be<f32> mAngleDeg;
    /* 0x124 */ be<f32> mDistFalloff;
    u8 padding128[8];
    Geometry mFlares[9];
    FlareMaterial mMaterials[9];
    u8 padding50EC[0xE8];
    TriangleBank mTriangles;
    u8 padding9C58[0x17C];
    ColorF mStarColor;
};
WWHD_OFFSET(LensPacket, mPositions, 0xA4);
WWHD_OFFSET(LensPacket, field_0x88, 0x118);
WWHD_OFFSET(LensPacket, field_0x8c, 0x11C);
WWHD_OFFSET(LensPacket, mAngleDeg, 0x120);
WWHD_OFFSET(LensPacket, mDistFalloff, 0x124);
struct LIGHT_INFLUENCE {
    /* 0x0 */ Vector mPos;
    /* 0xC */ be<s16> mColor[3];
    u8 padding12[0x2];
    /* 0x14 */ be<f32> mPower;
    /* 0x18 */ be<f32> mFluctuation;
};
WWHD_OFFSET(LIGHT_INFLUENCE, mPos, 0x0);
WWHD_OFFSET(LIGHT_INFLUENCE, mColor, 0xC);
WWHD_OFFSET(LIGHT_INFLUENCE, mPower, 0x14);
WWHD_OFFSET(LIGHT_INFLUENCE, mFluctuation, 0x18);
struct EF_THUNDER {
    /* 0x0 */ be<u8> mStatus;
    /* 0x1 */ be<u8> mState;
    u8 padding2[0x2];
    /* 0x4 */ be<s32> mMode;
    /* 0x8 */ be<f32> mFlashTimer;
    /* 0xC */ be<f32> field_0xc;
    /* 0x10 */ be<f32> field_0x10;
    u8 padding14[0x4];
    /* 0x18 */ LIGHT_INFLUENCE mLightInfluence;
};
WWHD_OFFSET(EF_THUNDER, mStatus, 0x0);
WWHD_OFFSET(EF_THUNDER, mState, 0x1);
WWHD_OFFSET(EF_THUNDER, mMode, 0x4);
WWHD_OFFSET(EF_THUNDER, mFlashTimer, 0x8);
WWHD_OFFSET(EF_THUNDER, field_0xc, 0xC);
WWHD_OFFSET(EF_THUNDER, field_0x10, 0x10);
WWHD_OFFSET(EF_THUNDER, mLightInfluence, 0x18);
struct Environment {
    u8 padding0[0xA34];
    /* 0xA34 */ be<u32> mpSunPacket;
    /* 0xA38 */ be<u32> mpSunlenzPacket;
    u8 paddingA3C[0x4];
    /* 0xA40 */ Count32 mRainCount;
    /* 0xA44 */ be<u32> mpRainPacket;
    u8 paddingA48[0x4];
    /* 0xA4C */ Count32 mSnowCount;
    /* 0xA50 */ be<u32> mpSnowPacket;
    u8 paddingA54[0x14];
    /* 0xA68 */ be<s32> mPoisonCount;
    /* 0xA6C */ be<u32> mpPoisonPacket;
    u8 paddingA70[0x40];
    /* 0xAB0 */ EF_THUNDER mThunderEff;
    u8 paddingAE4[0x5BC];
    /* 0x10A0 */ be<u8> field_0xc98;
    u8 padding10A1[0x2];
    /* 0x10A3 */ be<u8> mPoisonRoom;
};
WWHD_OFFSET(Environment, mpSunPacket, 0xA34);
WWHD_OFFSET(Environment, mpSunlenzPacket, 0xA38);
WWHD_OFFSET(Environment, mRainCount, 0xA40);
WWHD_OFFSET(Environment, mpRainPacket, 0xA44);
WWHD_OFFSET(Environment, mSnowCount, 0xA4C);
WWHD_OFFSET(Environment, mpSnowPacket, 0xA50);
WWHD_OFFSET(Environment, mPoisonCount, 0xA68);
WWHD_OFFSET(Environment, mpPoisonPacket, 0xA6C);
WWHD_OFFSET(Environment, mThunderEff, 0xAB0);
WWHD_OFFSET(Environment, field_0xc98, 0x10A0);
WWHD_OFFSET(Environment, mPoisonRoom, 0x10A3);
struct GameInfo {
    u8 padding0[0x12A0];
    /* 0x12A0 */ u8 mBgSpace[1];
    u8 padding12A1[0x3E93];
    /* 0x5134 */ be<char> mStartStageName[28];
    /* 0x5150 */ be<u32> mStageVtable;
    u8 padding5154[0x9A4];
    /* 0x5AF8 */ be<u32> mCamera;
    u8 padding5AFC[0x30];
    /* 0x5B2C */ be<u32> mPlayer;
    u8 padding5B30[0x4];
    /* 0x5B34 */ be<u32> mLinkPlayer;
    u8 padding5B38[0x46C];
    /* 0x5FA4 */ be<u32> mView;
};
WWHD_OFFSET(GameInfo, mBgSpace, 0x12A0);
WWHD_OFFSET(GameInfo, mStartStageName, 0x5134);
WWHD_OFFSET(GameInfo, mStageVtable, 0x5150);
WWHD_OFFSET(GameInfo, mCamera, 0x5AF8);
WWHD_OFFSET(GameInfo, mPlayer, 0x5B2C);
WWHD_OFFSET(GameInfo, mLinkPlayer, 0x5B34);
WWHD_OFFSET(GameInfo, mView, 0x5FA4);
struct Camera {
    u8 padding0[0xDC];
    /* 0xDC */ Vector mEye;
    /* 0xE8 */ Vector mCenter;
};
WWHD_OFFSET(Camera, mEye, 0xDC);
WWHD_OFFSET(Camera, mCenter, 0xE8);
struct StageVtable {
    u8 padding0[0x15C];
    /* 0x15C */ be<u32> getInfo;
};
WWHD_OFFSET(StageVtable, getInfo, 0x15C);
struct StageInfo {
    u8 padding0[0xC];
    /* 0xC */ be<u32> flags;
};
WWHD_OFFSET(StageInfo, flags, 0xC);
struct SafeStringVtable {
    u8 padding0[0x14];
    /* 0x14 */ be<u32> callback;
};
WWHD_OFFSET(SafeStringVtable, callback, 0x14);
struct ShaderResource {
    u8 padding0[0x24];
    /* 0x24 */ be<u32> relativeData;
};
WWHD_OFFSET(ShaderResource, relativeData, 0x24);
namespace sdk {
// 02555D0C [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R getEnvironment(A... args) {
    return gabi::call<R>(0x02555D0C, args...);
}
// 025200D4 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R getGameInfo(A... args) {
    return gabi::call<R>(0x025200D4, args...);
}
// 02563DD8 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R prepareProjection(A... args) {
    return gabi::call<R>(0x02563DD8, args...);
}
// 0256401C [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R dKy_set_eyevect_calc2(A... args) {
    return gabi::call<R>(0x0256401C, args...);
}
// 02563F64 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R dKyr_get_vectle_calc(A... args) {
    return gabi::call<R>(0x02563F64, args...);
}
// 025F1018 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R project(A... args) {
    return gabi::call<R>(0x025F1018, args...);
}
// 020195B0 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R cM_atan2s(A... args) {
    return gabi::call<R>(0x020195B0, args...);
}
// 028E8DE8 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R distanceSquared(A... args) {
    return gabi::call<R>(0x028E8DE8, args...);
}
// 028F4384 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R squareRoot(A... args) {
    return gabi::call<R>(0x028F4384, args...);
}
// 025F01D8 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R constructHeapGuard(A... args) {
    return gabi::call<R>(0x025F01D8, args...);
}
// 0273AE48 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R allocate(A... args) {
    return gabi::call<R>(0x0273AE48, args...);
}
// 0257FE7C [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R constructSnowPacket(A... args) {
    return gabi::call<R>(0x0257FE7C, args...);
}
// 025F02D0 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R reportHeapFailure(A... args) {
    return gabi::call<R>(0x025F02D0, args...);
}
// 025F0270 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R destroyHeapGuard(A... args) {
    return gabi::call<R>(0x025F0270, args...);
}
// 026124B0 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R getArchiveResource(A... args) {
    return gabi::call<R>(0x026124B0, args...);
}
// 027E2DC0 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R getShaderResource(A... args) {
    return gabi::call<R>(0x027E2DC0, args...);
}
// 027DFA24 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R getShaderUniform(A... args) {
    return gabi::call<R>(0x027DFA24, args...);
}
// 02580318 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R prepareSnowPacket(A... args) {
    return gabi::call<R>(0x02580318, args...);
}
// 020198D8 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R cM_rndF(A... args) {
    return gabi::call<R>(0x020198D8, args...);
}
// 0200ECD4 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R cLib_addCalc(A... args) {
    return gabi::call<R>(0x0200ECD4, args...);
}
// 02556910 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R setSkyKasumiColor(A... args) {
    return gabi::call<R>(0x02556910, args...);
}
// 025E19CC [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R startThunderSound(A... args) {
    return gabi::call<R>(0x025E19CC, args...);
}
// 025DADA4 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R createThunderActor(A... args) {
    return gabi::call<R>(0x025DADA4, args...);
}
// 0255B9C8 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R setEffectLight(A... args) {
    return gabi::call<R>(0x0255B9C8, args...);
}
// 0255BA9C [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R cutEffectLight(A... args) {
    return gabi::call<R>(0x0255BA9C, args...);
}
// 025548F0 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R actorAmbientColor(A... args) {
    return gabi::call<R>(0x025548F0, args...);
}
// 02554A04 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R actorDiffuseColor(A... args) {
    return gabi::call<R>(0x02554A04, args...);
}
// 02554D40 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R backgroundDiffuseColor(A... args) {
    return gabi::call<R>(0x02554D40, args...);
}
// 02554E54 [v d_kankyo_rain_movement_qa]
template <class R = void, class... A> inline R background1AmbientColor(A... args) {
    return gabi::call<R>(0x02554E54, args...);
}
} // namespace sdk
enum : u32 {
    SafeStringVtableAddress = 0x1004F3AC,
    Zero = 0x1004F528,
    CloudSmallOffset = 0x1004F544,
    One = 0x1004F550,
    Rate = 0x1004F554,
    ThunderActorChance = 0x1004F570,
    ThunderHeight = 0x1004F574,
    Half = 0x1004F57C,
    Increment = 0x1004F580,
    ThunderFogScale = 0x1004F584,
    Drag = 0x1004F588,
    SprayMinimumStep = 0x1004F58C,
    AlphaStep = 0x1004F590,
    ThunderSkyScale = 0x1004F598,
    HalfTurn = 0x1004F5DC,
    FlareMinimumVisibility = 0x1004F5E8,
    FlareEyeDistance = 0x1004F5EC,
    ScreenCenterX = 0x1004F5F0,
    ScreenCenterY = 0x1004F5F4,
    FlareAngleScale = 0x1004F5F8,
    FlareSizeGain = 0x1004F5FC,
    FlareSizeBase = 0x1004F600,
    FlareDisplacement = 0x1004F604,
    FlareStep = 0x1004F608,
    VolcanoStageName = 0x1004F758,
    DefaultSnowArchive = 0x1004F760,
    SnowHeapName = 0x1004F768,
    VolcanoSnowResource = 0x1004F784,
    DefaultSnowUniform = 0x1004F790,
    VolcanoArchive = 0x1004F79C,
    VolcanoSnowUniform = 0x1004F7AC,
    DefaultSnowResource = 0x1004F7BC,
    DirectionBias = 0x1004F844,
    ThunderNearScale = 0x1004F8D0,
    ThunderBackgroundScale = 0x1004F8E4,
    ThunderNearChance = 0x1004FB08,
    ThunderFarChance = 0x1004FD18,
    ThunderPower = 0x1004FD1C,
    ThunderColorScale = 0x1004FD20,
    ThunderFarFogScale = 0x1004FD24,
    ResourceManagerPointer = 0x101F4F7C
};
inline void safe_string_callback(SafeString *string) {
    gabi::call_ptr((u32)at<SafeStringVtable>(string->vtable)->callback, string);
}
inline bool equal_terminated_strings(u32 left, u32 right) {
    if (left == right)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 a = *at<be<u8>>(left + i), b = *at<be<u8>>(right + i);
        if (a != b)
            return false;
        if (!a)
            return true;
    }
    return false;
}
inline u32 snowPacket() { return at<Environment>(sdk::getEnvironment<u32>())->mpSnowPacket; }
inline void thunder_color(u32 target, u32 thunder, u32 scale, u32 red = 90, u32 green = 160, u32 blue = 245) {
    gabi::call(target, red, green, blue,
               gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, (f32)*at<be<f32>>(scale)));
}

union GroundVtableOrZero {
    be<u32> word;
    be<f32> value;
};
struct AshMotionWork {
    /* 0x0 */ be<f32> angleRange;
    /* 0x4 */ GroundVtableOrZero polyVtableOrHeight;
    /* 0x8 */ be<f32> minimumStep;
    /* 0xC */ be<f32> one;
    /* 0x10 */ union GroundVtableOrZero groundVtableOrZero;
    /* 0x14 */ be<u32> polygonVtable;
    /* 0x18 */ be<u32> groupVtable;
    /* 0x1C */ be<u32> groupAddress;
    /* 0x20 */ be<u32> passVtable;
    /* 0x24 */ Vector absolute;
    /* 0x30 */ Vector center;
    /* 0x3C */ Vector wind;
    /* 0x48 */ Vector direction;
    u8 padding54[0xC];
    /* 0x60 */ Vector wind2;
    /* 0x6C */ Vector viewDirection;
    /* 0x78 */ DOUBLE_POS doublePos;
    /* 0x90 */ GroundCheck ground;
    /* 0xE4 */ GroundCheck ground2;
};
WWHD_OFFSET(AshMotionWork, angleRange, 0x0);
WWHD_OFFSET(AshMotionWork, polyVtableOrHeight, 0x4);
WWHD_OFFSET(AshMotionWork, minimumStep, 0x8);
WWHD_OFFSET(AshMotionWork, one, 0xC);
WWHD_OFFSET(AshMotionWork, groundVtableOrZero, 0x10);
WWHD_OFFSET(AshMotionWork, polygonVtable, 0x14);
WWHD_OFFSET(AshMotionWork, groupVtable, 0x18);
WWHD_OFFSET(AshMotionWork, groupAddress, 0x1C);
WWHD_OFFSET(AshMotionWork, passVtable, 0x20);
WWHD_OFFSET(AshMotionWork, absolute, 0x24);
WWHD_OFFSET(AshMotionWork, center, 0x30);
WWHD_OFFSET(AshMotionWork, wind, 0x3C);
WWHD_OFFSET(AshMotionWork, direction, 0x48);
WWHD_OFFSET(AshMotionWork, wind2, 0x60);
WWHD_OFFSET(AshMotionWork, viewDirection, 0x6C);
WWHD_OFFSET(AshMotionWork, doublePos, 0x78);
WWHD_OFFSET(AshMotionWork, ground, 0x90);
WWHD_OFFSET(AshMotionWork, ground2, 0xE4);
WWHD_SIZE(AshMotionWork, 0x138);

inline void constructGround(GroundCheck *p, AshMotionWork *work) {
    gabi::call(0x02008E0C, p);
    u32 flags = ea(&p->pass), types = ea(&p->group);
    p->base.mPolyPassChk = flags;
    p->base.mGrpPassChk = types;
    work->polyVtableOrHeight.word = 0x1004F474;
    for (u32 i = 1; i <= 6; i++)
        p->pass.flags[i] = 0;
    work->groupAddress = types;
    p->base.cBgS_PolyInfo::__vtbl = 0x1004F484;
    p->pass.flags[0] = 1;
    work->groupVtable = 0x1004F494;
    work->polygonVtable = 0x1004F484;
    p->base.cBgS_Chk::__vtbl = 0x1004F474;
    p->group.mGrp = 15;
    p->group.__vtbl = 0x1004F494;
    work->passVtable = 0x1004F424;
    p->pass.vtable = 0x1004F4A4;
}
inline void absolutePosition(u32 output, u32 particle) {
    Vector *out = at<Vector>(output);
    SNOW_EFF *p = at<SNOW_EFF>(particle);
    for (u32 i = 0; i < 3; i++)
        out->channels[i] = fadds_ppc((f32)p->mBasePos.channels[i], (f32)p->mPos.channels[i]);
}
inline f32 ashDistance(u32 a, u32 b) {
    gabi::call(0x028E8DE8, at<void>(a), at<void>(b));
    return gabi::call<f32>(0x028F4384);
}

namespace sdk {
// 0257DB28 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R dKyw_get_wind_vecpow(A... args) {
    return gabi::call<R>(0x0257DB28, args...);
}
// 02008974 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R groundCross(A... args) {
    return gabi::call<R>(0x02008974, args...);
}
// 0257DAA8 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R dKyw_get_wind_vec(A... args) {
    return gabi::call<R>(0x0257DAA8, args...);
}
// 02578348 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R dKyw_get_wind_pow(A... args) {
    return gabi::call<R>(0x02578348, args...);
}
// 02563E48 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R vectle_calc(A... args) {
    return gabi::call<R>(0x02563E48, args...);
}
// 02010CFC [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R cM3d_VectorProduct2d(A... args) {
    return gabi::call<R>(0x02010CFC, args...);
}
// 02019918 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R cM_rndFX(A... args) {
    return gabi::call<R>(0x02019918, args...);
}
// 02019510 [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R cM_deg2s(A... args) {
    return gabi::call<R>(0x02019510, args...);
}
// 02563F0C [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R dKyr_get_vectle_calc2(A... args) {
    return gabi::call<R>(0x02563F0C, args...);
}
// 02008E0C [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R constructGroundBase(A... args) {
    return gabi::call<R>(0x02008E0C, args...);
}
// 02008DAC [v d_kankyo_rain_kazanbai_move]
template <class R = void, class... A> inline R destroyGroundBase(A... args) {
    return gabi::call<R>(0x02008DAC, args...);
}
} // namespace sdk

enum : u32 {
    BaseGroupVtable = 0x1004F3E4,
    BasePolyVtable = 0x1004F404,
    ObjectPassVtable = 0x1004F4A4,
    DoubleZero = 0x1004F530,
    WindDecay = 0x1004F56C,
    EyeDistance = 0x1004F6D0,
    ReflectionHeightRange = 0x1004F6D4,
    CloudWanderPhase = 0x1004F6D8,
    ReflectionOpacityScale = 0x1004F6EC,
    Radius = 0x1004F720,
    CloudPointMinimum = 0x1004F724,
    CloudPointThreshold = 0x1004F728,
    CloudPointGain = 0x1004F72C,
    CloudVerticalGain = 0x1004F730,
    CloudMode4Alpha = 0x1004F734,
    RotationStep = 0x1004F73C,
    InitialSpeed = 0x1004F744,
    AshEyeResetDistance = 0x1004F7D0,
    AshGroundX = 0x1004F7E4,
    AshGroundY = 0x1004F7E8,
    AshGroundZ = 0x1004F7EC,
    AshGravityBase = 0x1004F7F0,
    AshSettledDrift = 0x1004F7F4,
    AshWindRange = 0x1004F7F8,
    AshGravityRange = 0x1004F7FC,
    HousiSize = 0x1004F800,
    AshScatterResetDistance = 0x1004F804,
    AshSettledHeight = 0x1004F808,
    AshWarmBaseX = 0x1004F80C,
    AshWarmBaseZ = 0x1004F810,
    AshColdBaseX = 0x1004F814,
    AshColdBaseZ = 0x1004F818,
    AshColdRange = 0x1004F81C,
    FrameCounter = 0x101FF558,
    SinCosTable = 0x104A44F8
};

struct RoofCheck {
    dBgS_RoofChk base;
    be<u32> tail;
};
WWHD_SIZE(RoofCheck, 80);

namespace sdk {
// 024EE7AC [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R constructRoofCheck(A... args) {
    return gabi::call<R>(0x024EE7AC, args...);
}
// 024EF6E8 [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R roofCheck(A... args) {
    return gabi::call<R>(0x024EF6E8, args...);
}
// 02008B4C [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R destroyCheckBase(A... args) {
    return gabi::call<R>(0x02008B4C, args...);
}
// 02566974 [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R dKyr_get_rain_pos(A... args) {
    return gabi::call<R>(0x02566974, args...);
}
// 0256691C [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R dKyr_set_rain_base(A... args) {
    return gabi::call<R>(0x0256691C, args...);
}
// 028E8DD0 [v d_kankyo_rain_rainMove]
template <class R = void, class... A> inline R vectorSquareMagnitude(A... args) {
    return gabi::call<R>(0x028E8DD0, args...);
}
} // namespace sdk

enum : u32 {
    PolyPassVtable = 0x1004F424,
    GroundBaseVtable = 0x1004F474,
    GroundPolyVtable = 0x1004F484,
    GroundGroupVtable = 0x1004F494,
    RoofBaseVtable = 0x1004F4B4,
    RoofPassVtable = 0x1004F4D4,
    AshHeightRange = 0x1004F52C,
    AshOffsetStep = 0x1004F564,
    FarPlaneInset = 0x1004F594,
    ScalarOne = 0x1004F5D0,
    NoGroundHeight = 0x1004F630,
    HousiPhaseScale = 0x1004F6B8,
    MoonDistanceGain = 0x1004F6BC,
    RainFarFrontDistance = 0x1004F6C0,
    RainRadialLimitA = 0x1004F6C4,
    RainRadialLimitB = 0x1004F6C8,
    RainRadialLimitDefault = 0x1004F6CC,
    RainShift = 0x1004F6DC,
    RainFallBase = 0x1004F6E0,
    RainVerticalMotion = 0x1004F6E4,
    RainResetDistance = 0x1004F6E8,
    RainHeightFadeStage = 0x1004F6F0,
    RainOutdoorStage = 0x1004F6F8,
    RainSpecialStage = 0x1004F700,
    RainUnblockedStage = 0x1004F708,
    RainRadialStageA = 0x1004F710,
    RainRadialStageB = 0x1004F718
};

struct POISON_EFF {
    /* 0x0 */ be<u8> mStatus;
    u8 padding1[0x3];
    /* 0x4 */ Vector mPos;
    /* 0x10 */ be<f32> field_0x10;
    /* 0x14 */ be<f32> field_0x14;
    /* 0x18 */ be<f32> field_0x18;
    /* 0x1C */ be<f32> field_0x1c;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> mAlpha;
    /* 0x28 */ be<f32> mSize;
    /* 0x2C */ be<s16> field_0x2c;
    /* 0x2E */ be<s16> field_0x2e;
};
WWHD_OFFSET(POISON_EFF, mStatus, 0x0);
WWHD_OFFSET(POISON_EFF, mPos, 0x4);
WWHD_OFFSET(POISON_EFF, field_0x10, 0x10);
WWHD_OFFSET(POISON_EFF, field_0x14, 0x14);
WWHD_OFFSET(POISON_EFF, field_0x18, 0x18);
WWHD_OFFSET(POISON_EFF, field_0x1c, 0x1C);
WWHD_OFFSET(POISON_EFF, field_0x20, 0x20);
WWHD_OFFSET(POISON_EFF, mAlpha, 0x24);
WWHD_OFFSET(POISON_EFF, mSize, 0x28);
WWHD_OFFSET(POISON_EFF, field_0x2c, 0x2C);
WWHD_OFFSET(POISON_EFF, field_0x2e, 0x2E);
WWHD_SIZE(POISON_EFF, 0x30);
struct PoisonPacket {
    u8 padding0[0x98];
    /* 0x98 */ POISON_EFF mEff[1000];
    /* 0xBC18 */ Vector mBasePos;
    /* 0xBC24 */ Vector field_0xbb9c;
    /* 0xBC30 */ be<u32> mCount;
    u8 paddingBC34[0x4];
    /* 0xBC38 */ be<f32> mPhase;
};
WWHD_OFFSET(PoisonPacket, mEff, 0x98);
WWHD_OFFSET(PoisonPacket, mBasePos, 0xBC18);
WWHD_OFFSET(PoisonPacket, field_0xbb9c, 0xBC24);
WWHD_OFFSET(PoisonPacket, mCount, 0xBC30);
WWHD_OFFSET(PoisonPacket, mPhase, 0xBC38);

struct PoisonPattern {
    Vector mBasePos, mRange;
    be<f32> mSize, mRise;
    be<s32> mCount;
    be<s16> mTilt, mHeading;
    be<f32> mDrift;
    be<u32> reserved;
};
WWHD_SIZE(PoisonPattern, 48);
union PoisonPatternData {
    PoisonPattern value;
    be<u32> words[12];
};
union Scalar {
    be<u32> word;
    be<f32> value;
};

struct PoisonMotionWork {
    /* 0x0 */ Scalar rangeY;
    /* 0x4 */ Scalar hallLowerZ;
    /* 0x8 */ Scalar phaseStep;
    /* 0xC */ Scalar hallUpperZ;
    /* 0x10 */ Scalar nearDistance;
    /* 0x14 */ Scalar closestDistance;
    /* 0x18 */ Scalar middleLowerZ;
    /* 0x1C */ Scalar playerRadiusScale;
    /* 0x20 */ Scalar middleUpperZ;
    /* 0x24 */ Scalar swirl;
    /* 0x28 */ Scalar outerLowerZ;
    /* 0x2C */ Scalar corridorLowerX;
    /* 0x30 */ Scalar rise;
    /* 0x34 */ Scalar corridorUpperX;
    /* 0x38 */ Scalar windZ;
    /* 0x3C */ Scalar sizeWaveAmplitude;
    /* 0x40 */ Scalar rangeX;
    /* 0x44 */ Scalar edgeWidth;
    /* 0x48 */ Scalar rangeZ;
    /* 0x4C */ Vector total;
    u8 padding58[0x8];
    /* 0x60 */ Vector eye;
    /* 0x6C */ Vector influence;
    /* 0x78 */ Vector radialPosition;
    /* 0x84 */ Vector viewDirection;
    /* 0x90 */ Vector flatPosition;
    u8 padding9C[0x4];
    /* 0xA0 */ Scalar windY;
    u8 paddingA4[0x4];
    /* 0xA8 */ Vector wind;
    /* 0xB4 */ PoisonPatternData patterns[5];
};
WWHD_OFFSET(PoisonMotionWork, rangeY, 0x0);
WWHD_OFFSET(PoisonMotionWork, hallLowerZ, 0x4);
WWHD_OFFSET(PoisonMotionWork, phaseStep, 0x8);
WWHD_OFFSET(PoisonMotionWork, hallUpperZ, 0xC);
WWHD_OFFSET(PoisonMotionWork, nearDistance, 0x10);
WWHD_OFFSET(PoisonMotionWork, closestDistance, 0x14);
WWHD_OFFSET(PoisonMotionWork, middleLowerZ, 0x18);
WWHD_OFFSET(PoisonMotionWork, playerRadiusScale, 0x1C);
WWHD_OFFSET(PoisonMotionWork, middleUpperZ, 0x20);
WWHD_OFFSET(PoisonMotionWork, swirl, 0x24);
WWHD_OFFSET(PoisonMotionWork, outerLowerZ, 0x28);
WWHD_OFFSET(PoisonMotionWork, corridorLowerX, 0x2C);
WWHD_OFFSET(PoisonMotionWork, rise, 0x30);
WWHD_OFFSET(PoisonMotionWork, corridorUpperX, 0x34);
WWHD_OFFSET(PoisonMotionWork, windZ, 0x38);
WWHD_OFFSET(PoisonMotionWork, sizeWaveAmplitude, 0x3C);
WWHD_OFFSET(PoisonMotionWork, rangeX, 0x40);
WWHD_OFFSET(PoisonMotionWork, edgeWidth, 0x44);
WWHD_OFFSET(PoisonMotionWork, rangeZ, 0x48);
WWHD_OFFSET(PoisonMotionWork, total, 0x4C);
WWHD_OFFSET(PoisonMotionWork, eye, 0x60);
WWHD_OFFSET(PoisonMotionWork, influence, 0x6C);
WWHD_OFFSET(PoisonMotionWork, radialPosition, 0x78);
WWHD_OFFSET(PoisonMotionWork, viewDirection, 0x84);
WWHD_OFFSET(PoisonMotionWork, flatPosition, 0x90);
WWHD_OFFSET(PoisonMotionWork, windY, 0xA0);
WWHD_OFFSET(PoisonMotionWork, wind, 0xA8);
WWHD_OFFSET(PoisonMotionWork, patterns, 0xB4);
WWHD_SIZE(PoisonMotionWork, 0x1A4);

struct PoisonPlayer {
    u8 padding0[0x314];
    /* 0x314 */ Vector mCurrentPos;
    u8 padding320[0xA0];
    /* 0x3C0 */ be<u32> mResetFlg0;
};
WWHD_OFFSET(PoisonPlayer, mCurrentPos, 0x314);
WWHD_OFFSET(PoisonPlayer, mResetFlg0, 0x3C0);

inline f32 poisonSine(u32 angle) { return at<SinCos>(SinCosTable)[(u16)angle >> 3].sine; }
inline f32 poisonCosine(u32 angle) { return at<SinCos>(SinCosTable)[(u16)angle >> 3].cosine; }
inline void poisonTotal(u32 output, u32 base, u32 slot) {
    for (u32 i = 0; i < 3; i++)
        at<Vector>(output)->channels[i] =
            fadds_ppc((f32)at<Vector>(base)->channels[i], (f32)at<POISON_EFF>(slot)->mPos.channels[i]);
}
inline f32 poisonDistance(u32 a, u32 b) {
    f32 d = sdk::distanceSquared<f32>(a, b);
    return sdk::squareRoot<f32>(d);
}
inline void poisonCopy(u32 output, u32 input) {
    for (u32 i = 0; i < 3; i++)
        at<Vector>(output)->words[i] = (u32)at<Vector>(input)->words[i];
}

namespace sdk {
// 0201AD78 [v d_kankyo_rain_poison_move]
template <class R = void, class... A> inline R vectorAdd(A... args) {
    return gabi::call<R>(0x0201AD78, args...);
}
// 0257E128 [v d_kankyo_rain_poison_move]
template <class R = void, class... A> inline R dKyw_get_AllWind_vec(A... args) {
    return gabi::call<R>(0x0257E128, args...);
}
// 02560704 [v d_kankyo_rain_poison_move]
template <class R = void, class... A> inline R dKy_contrast_flg_get(A... args) {
    return gabi::call<R>(0x02560704, args...);
}
} // namespace sdk

enum : u32 {
    SpiralResetScale = 0x1004F540,
    ReflectionHeightMax = 0x1004F558,
    PoisonSwirlAngleScale = 0x1004F59C,
    PoisonMiddleUpperZ = 0x1004F5C0,
    Fall = 0x1004F740,
    PoisonPhaseRange = 0x1004F748,
    PoisonPlayerRadiusScale = 0x1004F854,
    CloudRotationWrap = 0x1004F8C4,
    PoisonPhaseStep = 0x1004F924,
    PoisonInfluenceLimit = 0x1004F928,
    PoisonInfluenceGain = 0x1004F92C,
    PoisonCorridorUpperX = 0x1004F930,
    PoisonCorridorLowerX = 0x1004F934,
    PoisonMiddleLowerZ = 0x1004F938,
    PoisonHallUpperZ = 0x1004F93C,
    PoisonHallLowerZ = 0x1004F940,
    PoisonAngleRange = 0x1004F944,
    PoisonWrapScale = 0x1004F948,
    PoisonOuterBoundaryX = 0x1004F94C,
    PoisonMiddleBoundaryX = 0x1004F950,
    PoisonHallMaximumX = 0x1004F954,
    PoisonHallMinimumX = 0x1004F958,
    PoisonCorridorBoundaryZ = 0x1004F95C,
    PreviousPoisonRoom = 0x101E99F0,
    CurrentRoom = 0x1047E6C8
};

struct LensDrawWork {
    /* 0x0 */ Scalar half;
    /* 0x4 */ Scalar cornerZ;
    /* 0x8 */ Scalar unit;
    /* 0xC */ Scalar tenth;
    /* 0x10 */ Scalar visibilitySquared;
    /* 0x14 */ Scalar visibility;
    /* 0x18 */ Scalar zeroOrScale;
    /* 0x1C */ Scalar green;
    /* 0x20 */ Scalar red;
    /* 0x24 */ Scalar alphaRate;
    u8 padding28[0x14];
    /* 0x3C */ Scalar fade;
    u8 padding40[0x8];
    /* 0x48 */ Scalar visibilityPower;
    /* 0x4C */ Scalar falloff;
    u8 padding50[0x8];
    /* 0x58 */ Vector local;
    /* 0x64 */ Vector transformed;
    /* 0x70 */ Color color;
    /* 0x74 */ Vector position;
    /* 0x80 */ Vector cornerA;
    /* 0x8C */ Vector cornerB;
    /* 0x98 */ Vector cornerC;
    /* 0xA4 */ Vector cornerD;
    /* 0xB0 */ Color secondaryColor;
    /* 0xB4 */ Vector eye;
    /* 0xC0 */ be<s16> colorAngles[2];
    u8 paddingC4[0x4];
    /* 0xC8 */ ColorF convertedColor;
    /* 0xD8 */ ColorF convertedSecondaryColor;
    /* 0xE8 */ Vector center;
    /* 0xF4 */ Matrix matrix;
    /* 0x124 */ Vector screenDirection;
    /* 0x130 */ Vector projected;
    /* 0x13C */ Matrix rotation;
    /* 0x16C */ ProjectionParameters projection;
};
WWHD_OFFSET(LensDrawWork, half, 0x0);
WWHD_OFFSET(LensDrawWork, cornerZ, 0x4);
WWHD_OFFSET(LensDrawWork, unit, 0x8);
WWHD_OFFSET(LensDrawWork, tenth, 0xC);
WWHD_OFFSET(LensDrawWork, visibilitySquared, 0x10);
WWHD_OFFSET(LensDrawWork, visibility, 0x14);
WWHD_OFFSET(LensDrawWork, zeroOrScale, 0x18);
WWHD_OFFSET(LensDrawWork, green, 0x1C);
WWHD_OFFSET(LensDrawWork, red, 0x20);
WWHD_OFFSET(LensDrawWork, alphaRate, 0x24);
WWHD_OFFSET(LensDrawWork, fade, 0x3C);
WWHD_OFFSET(LensDrawWork, visibilityPower, 0x48);
WWHD_OFFSET(LensDrawWork, falloff, 0x4C);
WWHD_OFFSET(LensDrawWork, local, 0x58);
WWHD_OFFSET(LensDrawWork, transformed, 0x64);
WWHD_OFFSET(LensDrawWork, color, 0x70);
WWHD_OFFSET(LensDrawWork, position, 0x74);
WWHD_OFFSET(LensDrawWork, cornerA, 0x80);
WWHD_OFFSET(LensDrawWork, cornerB, 0x8C);
WWHD_OFFSET(LensDrawWork, cornerC, 0x98);
WWHD_OFFSET(LensDrawWork, cornerD, 0xA4);
WWHD_OFFSET(LensDrawWork, secondaryColor, 0xB0);
WWHD_OFFSET(LensDrawWork, eye, 0xB4);
WWHD_OFFSET(LensDrawWork, colorAngles, 0xC0);
WWHD_OFFSET(LensDrawWork, convertedColor, 0xC8);
WWHD_OFFSET(LensDrawWork, convertedSecondaryColor, 0xD8);
WWHD_OFFSET(LensDrawWork, center, 0xE8);
WWHD_OFFSET(LensDrawWork, matrix, 0xF4);
WWHD_OFFSET(LensDrawWork, screenDirection, 0x124);
WWHD_OFFSET(LensDrawWork, projected, 0x130);
WWHD_OFFSET(LensDrawWork, rotation, 0x13C);
WWHD_OFFSET(LensDrawWork, projection, 0x16C);
WWHD_SIZE(LensDrawWork, 0x194);
struct LensView {
    u8 padding0[0x1E4];
    /* 0x1E4 */ Matrix mInverseView;
};
WWHD_OFFSET(LensView, mInverseView, 0x1E4);

inline void clear_cache(u32 address, u32 size) {
    u32 end = address + size;
    for (u32 cursor = address; cursor < end; cursor += 32)
        for (u32 word = 0; word < 8; word++)
            at<CacheLine>(cursor & ~31u)->words[word] = 0;
}
inline f32 lensSine(s32 angle) { return at<SinCos>(SinCosTable)[(u16)angle >> 3].sine; }
inline f32 lensCosine(s32 angle) { return at<SinCos>(SinCosTable)[(u16)angle >> 3].cosine; }

namespace sdk {
// 028E91EC [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R matrixCopy(A... args) {
    return gabi::call<R>(0x028E91EC, args...);
}
// 028E98C0 [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R matrixRotateAxis(A... args) {
    return gabi::call<R>(0x028E98C0, args...);
}
// 028E9108 [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R matrixConcat(A... args) {
    return gabi::call<R>(0x028E9108, args...);
}
// 0257015C [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R convertColor(A... args) {
    return gabi::call<R>(0x0257015C, args...);
}
// 028E8F64 [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R matrixTransform(A... args) {
    return gabi::call<R>(0x028E8F64, args...);
}
// 027B5E94 [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R flushBuffer(A... args) {
    return gabi::call<R>(0x027B5E94, args...);
}
// 0257EF00 [v d_kankyo_rain_lenzflare]
template <class R = void, class... A> inline R drawLenzPacket(A... args) {
    return gabi::call<R>(0x0257EF00, args...);
}
} // namespace sdk

enum : u32 {
    LensMiddleAlpha = 0x1004F5AC,
    LensEyeDistance = 0x1004F614,
    LensRotationScale = 0x1004FB7C,
    LensFalloffRange = 0x1004FBC0,
    LensAngleAScale = 0x1004FBC8,
    LensAngleABias = 0x1004FBCC,
    LensAngleBScale = 0x1004FBD0,
    LensAngleBBias = 0x1004FBD4,
    LensGainBias = 0x1004FBD8,
    LensRayScale = 0x1004FBDC,
    LensWidthScale = 0x1004FBE0,
    LensOscillationScale = 0x1004FBE4,
    LensBaseSizeGain = 0x1004FBE8,
    LensLargeSizeScale = 0x1004FBEC
};

enum : u32 {
    FlareAlphaTable = 0x1004FBF0,
    FlareRedTable = 0x1004FC14,
    FlareGreenTable = 0x1004FC38,
    FlareSizeTable = 0x101E9A00
};

enum : u32 { PoisonPatterns = 0x1004F9A0, RoomPatternMap = 0x1004F960 };
enum ThunderState : u8 {
    ThunderIdle = 0,
    ThunderNearFlash = 1,
    ThunderNearFade = 2,
    ThunderFarFlash = 11,
    ThunderFarFade = 12
};
enum : u32 {
    ThunderSkyColor = 0x025567FC,
    ThunderFogColor = 0x02556A24,
    ThunderBackgroundAmbientColor = 0x02554C2C,
    ThunderSound = 0x69F7,
    ThunderActor = 0x1B4,
    SnowHeapSize = 0xA26D10,
    VolcanoSnowHeapSize = 0x14E4D10,
    SnowPacketSize = 0x92B7E0
};
WWHD_SIZE(PolyPass, 12);
WWHD_OFFSET(PolyPass, flags, 4);
WWHD_OFFSET(GroundCheck, pass, 0x40);
WWHD_OFFSET(GroundCheck, group, 0x4C);
WWHD_SIZE(Scalar, 4);
WWHD_SIZE(Count32, 4);
WWHD_SIZE(SinCos, 8);
WWHD_OFFSET(BufferSlot, size, 0x150);
WWHD_OFFSET(BufferSlot, drawObject, 0x158);
WWHD_OFFSET(Geometry, selected, 0x4A8);
WWHD_OFFSET(FlareMaterial, mColor, 0x168);
WWHD_OFFSET(FlareMaterial, mSecondaryColor, 0x178);
WWHD_OFFSET(FlareVertex, mU, 0x38);
WWHD_OFFSET(FlareVertex, mV, 0x3C);
WWHD_OFFSET(LensPacket, mFlares, 0x130);
WWHD_OFFSET(LensPacket, mMaterials, 0x2BF0);
WWHD_OFFSET(LensPacket, mTriangles, 0x51D4);
WWHD_OFFSET(LensPacket, mStarColor, 0x9DD4);
WWHD_OFFSET(PoisonPattern, mSize, 24);
WWHD_OFFSET(PoisonPattern, mCount, 32);
WWHD_OFFSET(PoisonPattern, mHeading, 38);
enum ParticleStatus : u8 {
    ParticleInactive = 0,
    ParticleMoving = 1,
    ParticleSettled = 2,
    ParticleSpecial = 3
};
enum PoisonLayout : s32 {
    PoisonForwardEye = 0,
    PoisonBox = 1,
    PoisonCircular = 2,
    PoisonCorridor = 3,
    PoisonHall = 4
};
enum : u32 {
    AshWindBase = CloudPointGain,
    AshAngleRange = CloudWanderPhase,
    AshTimerRange = ReflectionHeightRange,
    AshShellJitter = ReflectionOpacityScale,
    AshMinimumFadeStep = CloudPointMinimum,
    AshDistanceBias = CloudPointThreshold,
    AshWaveStep = RotationStep,
    AshTamariDrift = CloudMode4Alpha,
    AshTamariGravityBase = InitialSpeed,
    AshWarmRange = Radius,
    AshTamariFadeHeight = WindDecay,
    AshVerticalWaveScale = HousiSize,
    AshScatterRadius = EyeDistance,
    PoisonPhaseWrap = CloudRotationWrap,
    PoisonRapidRise = CloudVerticalGain,
    PoisonWindGain = HousiPhaseScale,
    PoisonInfluenceDecay = Fall,
    PoisonForwardEyeDistance = SpiralResetScale,
    PoisonEdgeWidth = ReflectionHeightMax,
    PoisonOuterLowerZ = RainRadialLimitA,
    PoisonInitialPhaseRange = CloudWanderPhase,
    PoisonSwirlBase = InitialSpeed,
    PoisonNearFadeDistance = AshColdRange,
    PoisonInfluenceHeight = AshHeightRange,
    RainEyeChangeThreshold = HousiPhaseScale,
    RainMotionGain = ReflectionHeightRange,
    RainAngleRange = CloudWanderPhase,
    RainResetHeight = AshHeightRange,
    RainScatterRadius = EyeDistance,
    RainShellJitter = ReflectionOpacityScale,
    LensFullTurn = CloudWanderPhase,
    LensColorChannelMax = ReflectionHeightMax,
    LensRayScaleBase = Fall,
    LensRayAmplitude = InitialSpeed,
    LensRingRadius = AshHeightRange,
    LensRadialBias = HousiSize
};
enum : u32 { PoisonCurseFlag = 0x00100000 }; // daPyRFlg0_POISON_CURSE
} // namespace RainMotion
