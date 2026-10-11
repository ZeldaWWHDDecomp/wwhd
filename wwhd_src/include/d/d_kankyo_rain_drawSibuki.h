// Rain drawing and particle motion.
// HD: renderer buffers replace GX immediate drawing; explicit guest views preserve the GHS layouts.
#pragma once
#include "bindings.h"
#include <bit>
namespace RainDraw {
using namespace gabi;
union Vector {
    cXyz value;
    be<f32> channels[3];
    be<u32> words[3];
};
struct HOUSI_EFF {
    be<u8> mStatus;
    u8 padding[3];
    Vector mPos, mBasePos, mSpeed, mScale;
    be<f32> field_0x34, field_0x38;
    be<u16> field_0x3c;
    u8 reserved[2];
    be<f32> mAlpha, field_0x44, field_0x48;
    be<u16> field_0x4c;
    u8 tail[2];
};
struct HousiPacket {
    u8 header[0x9C];
    HOUSI_EFF mEffect[300];
    u8 countData[8];
    be<f32> field_0x5ddc; // HD: GC field shifts to the verified packet extension.
    be<s16> mCount;
    u8 tail[2];
};

struct Matrix {
    be<f32> cells[12];
};
struct ColorF {
    be<f32> channels[4];
};
union Color {
    be<u32> rgba;
    be<u8> channels[4];
};
union Quad {
    Vector vertices[4];
    be<u32> words[4][3];
};
struct ProjectParams {
    u8 projectionData[40];
};
struct SafeString {
    be<u32> text, vtable;
};
union CacheLine {
    be<u32> words[8];
    be<u8> bytes[32];
};
struct BufferSlot {
    be<u32> buffer;
    u8 flushObject[0x14C];
    be<u32> size;
    u8 drawPadding[4];
    u8 drawObject[0xFC];
};
struct Geometry {
    BufferSlot slots[2];
    be<u32> selected;
    u8 tail[20];
};
struct StarGeometry {
    BufferSlot slots[2][2];
    be<u32> selected;
    u8 tail[20];
};
struct Vertex {
    Vector position;
    be<f32> u, v;
};
struct Uniform {
    u8 shaderData[0x370];
};
struct Texture {
    u8 samplerData[0x198];
};
struct GeometryBank {
    BufferSlot slots[600];
    be<u32> selected;
    u8 tail[20];
};
WWHD_SIZE(Matrix, 48);
WWHD_SIZE(ColorF, 16);
WWHD_SIZE(Color, 4);
WWHD_SIZE(Quad, 48);
WWHD_SIZE(BufferSlot, 0x254);
WWHD_OFFSET(BufferSlot, drawObject, 0x158);
WWHD_SIZE(Geometry, 0x4C0);
WWHD_SIZE(StarGeometry, 0x968);
WWHD_SIZE(Vertex, 20);
WWHD_SIZE(GeometryBank, 0x574F8);
struct WAVE_EFF {
    /* 0x0 */ Vector mPos;
    /* 0xC */ Vector mBasePos;
    /* 0x18 */ be<f32> mSpeed;
    /* 0x1C */ be<f32> mScale;
    /* 0x20 */ be<f32> mCounterSpeed;
    /* 0x24 */ be<f32> mCounter;
    /* 0x28 */ be<f32> mAlpha;
    /* 0x2C */ be<f32> mStrengthEnv;
    /* 0x30 */ be<s16> field_0x30;
    /* 0x32 */ be<s16> field_0x32;
    /* 0x34 */ be<u8> mStatus;
    u8 tail[0x3];
};
WWHD_OFFSET(WAVE_EFF, mPos, 0x0);
WWHD_OFFSET(WAVE_EFF, mBasePos, 0xC);
WWHD_OFFSET(WAVE_EFF, mSpeed, 0x18);
WWHD_OFFSET(WAVE_EFF, mScale, 0x1C);
WWHD_OFFSET(WAVE_EFF, mCounterSpeed, 0x20);
WWHD_OFFSET(WAVE_EFF, mCounter, 0x24);
WWHD_OFFSET(WAVE_EFF, mAlpha, 0x28);
WWHD_OFFSET(WAVE_EFF, mStrengthEnv, 0x2C);
WWHD_OFFSET(WAVE_EFF, field_0x30, 0x30);
WWHD_OFFSET(WAVE_EFF, field_0x32, 0x32);
WWHD_OFFSET(WAVE_EFF, mStatus, 0x34);
WWHD_SIZE(WAVE_EFF, 0x38);
struct VRKUMO_EFF {
    /* 0x0 */ be<u8> mStatus;
    u8 padding1[0x3];
    /* 0x4 */ Vector mPosition;
    u8 padding10[0xC];
    /* 0x1C */ be<f32> mHeight;
    /* 0x20 */ be<f32> mAlpha;
    /* 0x24 */ be<f32> mDistFalloff;
    /* 0x28 */ be<f32> mSpeed;
};
WWHD_OFFSET(VRKUMO_EFF, mStatus, 0x0);
WWHD_OFFSET(VRKUMO_EFF, mPosition, 0x4);
WWHD_OFFSET(VRKUMO_EFF, mHeight, 0x1C);
WWHD_OFFSET(VRKUMO_EFF, mAlpha, 0x20);
WWHD_OFFSET(VRKUMO_EFF, mDistFalloff, 0x24);
WWHD_OFFSET(VRKUMO_EFF, mSpeed, 0x28);
WWHD_SIZE(VRKUMO_EFF, 0x2C);
struct RainPacket {
    u8 padding0[0x378C];
    /* 0x378C */ be<f32> mSibukiAlpha;
    u8 padding3790[0xC];
    /* 0x379C */ be<u8> mStatus;
    u8 padding379D[0x128EFF];
    /* 0x12C69C */ Geometry mSibukiGeometry[125];
    u8 padding15185C[0x5AD24];
    /* 0x1AC580 */ Uniform mSibukiUniform;
};
WWHD_OFFSET(RainPacket, mSibukiAlpha, 0x378C);
WWHD_OFFSET(RainPacket, mStatus, 0x379C);
WWHD_OFFSET(RainPacket, mSibukiGeometry, 0x12C69C);
WWHD_OFFSET(RainPacket, mSibukiUniform, 0x1AC580);
struct StarPacket {
    u8 padding0[0xC0];
    /* 0xC0 */ be<f32> mSin;
    u8 paddingC4[0x8];
    /* 0xCC */ be<s16> mEffectNum;
    u8 paddingCE[0x2];
    /* 0xD0 */ be<u32> mRot;
    /* 0xD4 */ be<u8> mDrawn[2000];
    u8 padding8A4[0x4];
    /* 0x8A8 */ StarGeometry mGeometry[2000];
    u8 padding498528[0x20E0];
    /* 0x49A608 */ be<u8> mColorKind[2000];
};
WWHD_OFFSET(StarPacket, mSin, 0xC0);
WWHD_OFFSET(StarPacket, mEffectNum, 0xCC);
WWHD_OFFSET(StarPacket, mRot, 0xD0);
WWHD_OFFSET(StarPacket, mDrawn, 0xD4);
WWHD_OFFSET(StarPacket, mGeometry, 0x8A8);
WWHD_OFFSET(StarPacket, mColorKind, 0x49A608);
struct SunPacket {
    u8 padding0[0x98];
    /* 0x98 */ Vector mPos[2];
    u8 paddingB0[0x14];
    /* 0xC4 */ be<u8> field_0x3c;
    /* 0xC5 */ be<u8> field_0x3d;
    u8 paddingC6[0x2];
    /* 0xC8 */ be<f32> mVisibility;
    /* 0xCC */ be<f32> mSunAlpha;
    /* 0xD0 */ be<f32> mMoonAlpha;
    u8 paddingD4[0x1C];
    /* 0xF0 */ Geometry mMoonGeometry[2];
    /* 0xA70 */ Geometry mSunGeometry[2];
};
WWHD_OFFSET(SunPacket, mPos, 0x98);
WWHD_OFFSET(SunPacket, field_0x3c, 0xC4);
WWHD_OFFSET(SunPacket, field_0x3d, 0xC5);
WWHD_OFFSET(SunPacket, mVisibility, 0xC8);
WWHD_OFFSET(SunPacket, mSunAlpha, 0xCC);
WWHD_OFFSET(SunPacket, mMoonAlpha, 0xD0);
WWHD_OFFSET(SunPacket, mMoonGeometry, 0xF0);
WWHD_OFFSET(SunPacket, mSunGeometry, 0xA70);
struct LensPacket {
    u8 padding0[0x124];
    /* 0x124 */ be<f32> mDistFalloff;
};
WWHD_OFFSET(LensPacket, mDistFalloff, 0x124);
struct WavePacket {
    u8 padding0[0xA0];
    /* 0xA0 */ WAVE_EFF mEff[300];
    /* 0x4240 */ be<f32> mSkewWidth;
    /* 0x4244 */ be<f32> mSkewDir;
    u8 padding4248[0x4];
    /* 0x424C */ Geometry mGeometry[300];
    u8 padding5D34C[0x40844];
    /* 0x9DB90 */ be<u8> mDrawn[300];
};
WWHD_OFFSET(WavePacket, mEff, 0xA0);
WWHD_OFFSET(WavePacket, mSkewWidth, 0x4240);
WWHD_OFFSET(WavePacket, mSkewDir, 0x4244);
WWHD_OFFSET(WavePacket, mGeometry, 0x424C);
WWHD_OFFSET(WavePacket, mDrawn, 0x9DB90);
struct CloudPacket {
    u8 padding0[0xA4];
    /* 0xA4 */ VRKUMO_EFF mInst[100];
    /* 0x11D4 */ be<f32> mLoopCount;
    /* 0x11D8 */ be<u32> mResource;
    /* 0x11DC */ GeometryBank mGeometry[2];
    /* 0xAFBCC */ u8 mSetup[0x41C];
    /* 0xAFFE8 */ Uniform mUniforms[100];
    u8 paddingC57A8[0x1B0];
    /* 0xC5958 */ Texture mTextures[3];
};
WWHD_OFFSET(CloudPacket, mInst, 0xA4);
WWHD_OFFSET(CloudPacket, mLoopCount, 0x11D4);
WWHD_OFFSET(CloudPacket, mResource, 0x11D8);
WWHD_OFFSET(CloudPacket, mGeometry, 0x11DC);
WWHD_OFFSET(CloudPacket, mSetup, 0xAFBCC);
WWHD_OFFSET(CloudPacket, mUniforms, 0xAFFE8);
WWHD_OFFSET(CloudPacket, mTextures, 0xC5958);
struct Environment {
    u8 padding0[0x20];
    /* 0x20 */ be<s16> mBaseLightRed;
    u8 padding22[0x9C2];
    /* 0x9E4 */ be<f32> mWaveScale;
    u8 padding9E8[0x8];
    /* 0x9F0 */ be<f32> mWaveScaleBottom;
    /* 0x9F4 */ be<f32> mWaveFlatInter;
    /* 0x9F8 */ be<s16> mWaveCount;
    u8 padding9FA[0x3A];
    /* 0xA34 */ gptr<SunPacket> mpSunPacket;
    /* 0xA38 */ gptr<LensPacket> mpSunlenzPacket;
    u8 paddingA3C[0x4];
    /* 0xA40 */ be<s32> mRainCount;
    /* 0xA44 */ gptr<RainPacket> mpRainPacket;
    u8 paddingA48[0x4];
    /* 0xA4C */ be<u32> mSnowCount;
    u8 paddingA50[0x10];
    /* 0xA60 */ gptr<StarPacket> mpStarPacket;
    u8 paddingA64[0x10];
    /* 0xA74 */ be<u32> mHousiCount;
    /* 0xA78 */ gptr<HousiPacket> mpHousiPacket;
    u8 paddingA7C[0x14];
    /* 0xA90 */ be<f32> mVrkumoStrength;
    /* 0xA94 */ gptr<CloudPacket> mpVrkumoPacket;
    u8 paddingA98[0x8];
    /* 0xAA0 */ gptr<WavePacket> mpWavePacket;
    u8 paddingAA4[0xA0];
    /* 0xB44 */ Vector mMoonPos;
    u8 paddingB50[0x3C];
    /* 0xB8C */ Color mFogColor;
    u8 paddingB90[0x8];
    /* 0xB98 */ Color mVrkumoColor;
    /* 0xB9C */ Color mVrkumoCenterColor;
    u8 paddingBA0[0x480];
    /* 0x1020 */ be<f32> mCurTime;
    u8 padding1024[0x6E];
    /* 0x1092 */ be<u8> mVrboxInvisible;
    u8 padding1093[0x49];
    /* 0x10DC */ be<f32> mCloudColorScale;
    /* 0x10E0 */ be<f32> mCloudCenterColorScale;
};
WWHD_OFFSET(Environment, mBaseLightRed, 0x20);
WWHD_OFFSET(Environment, mWaveScale, 0x9E4);
WWHD_OFFSET(Environment, mWaveScaleBottom, 0x9F0);
WWHD_OFFSET(Environment, mWaveFlatInter, 0x9F4);
WWHD_OFFSET(Environment, mWaveCount, 0x9F8);
WWHD_OFFSET(Environment, mpSunPacket, 0xA34);
WWHD_OFFSET(Environment, mpSunlenzPacket, 0xA38);
WWHD_OFFSET(Environment, mRainCount, 0xA40);
WWHD_OFFSET(Environment, mpRainPacket, 0xA44);
WWHD_OFFSET(Environment, mSnowCount, 0xA4C);
WWHD_OFFSET(Environment, mpStarPacket, 0xA60);
WWHD_OFFSET(Environment, mHousiCount, 0xA74);
WWHD_OFFSET(Environment, mpHousiPacket, 0xA78);
WWHD_OFFSET(Environment, mVrkumoStrength, 0xA90);
WWHD_OFFSET(Environment, mpVrkumoPacket, 0xA94);
WWHD_OFFSET(Environment, mpWavePacket, 0xAA0);
WWHD_OFFSET(Environment, mMoonPos, 0xB44);
WWHD_OFFSET(Environment, mFogColor, 0xB8C);
WWHD_OFFSET(Environment, mVrkumoColor, 0xB98);
WWHD_OFFSET(Environment, mVrkumoCenterColor, 0xB9C);
WWHD_OFFSET(Environment, mCurTime, 0x1020);
WWHD_OFFSET(Environment, mVrboxInvisible, 0x1092);
WWHD_OFFSET(Environment, mCloudColorScale, 0x10DC);
WWHD_OFFSET(Environment, mCloudCenterColorScale, 0x10E0);
struct GameInfo {
    u8 padding0[0x5134];
    /* 0x5134 */ be<char> mStageName[0x1C];
    /* 0x5150 */ be<u32> mStageVtable;
    u8 padding5154[0x9A4];
    /* 0x5AF8 */ be<u32> camera;
    u8 padding5AFC[0x4A8];
    /* 0x5FA4 */ be<u32> view;
};
WWHD_OFFSET(GameInfo, mStageName, 0x5134);
WWHD_OFFSET(GameInfo, mStageVtable, 0x5150);
WWHD_OFFSET(GameInfo, camera, 0x5AF8);
WWHD_OFFSET(GameInfo, view, 0x5FA4);
struct Camera {
    u8 padding0[0xDC];
    /* 0xDC */ Vector mEye;
    /* 0xE8 */ Vector mCenter;
    u8 paddingF4[0xC];
    /* 0x100 */ be<s16> mBank;
    u8 padding102[0x132];
    /* 0x234 */ be<s16> mAngleX;
};
WWHD_OFFSET(Camera, mEye, 0xDC);
WWHD_OFFSET(Camera, mCenter, 0xE8);
WWHD_OFFSET(Camera, mBank, 0x100);
WWHD_OFFSET(Camera, mAngleX, 0x234);
struct View {
    u8 padding0[0xD4];
    /* 0xD4 */ be<f32> mFovy;
    u8 paddingD8[0x10C];
    /* 0x1E4 */ Matrix mRotation;
};
WWHD_OFFSET(View, mFovy, 0xD4);
WWHD_OFFSET(View, mRotation, 0x1E4);
struct SunWork {
    /* 0x0 */ be<f32> degree;
    /* 0x4 */ be<f32> zero;
    /* 0x8 */ be<f32> alphaScale;
    /* 0xC */ be<f32> uvOne;
    /* 0x10 */ Vector input;
    /* 0x1C */ Color color;
    /* 0x20 */ Vector output;
    /* 0x2C */ Vector sun;
    /* 0x38 */ Color secondary;
    /* 0x3C */ Quad quad;
    u8 padding6C[0xC];
    /* 0x78 */ Vector moon;
    /* 0x84 */ SafeString stage;
    /* 0x8C */ SafeString current;
    /* 0x94 */ Vector relative;
    /* 0xA0 */ Vector cameraDirection;
    /* 0xAC */ Vector sunDirection;
    /* 0xB8 */ Matrix matrix;
    /* 0xE8 */ Matrix rotation;
    /* 0x118 */ be<u32> dayScales[7];
};
WWHD_OFFSET(SunWork, degree, 0x0);
WWHD_OFFSET(SunWork, zero, 0x4);
WWHD_OFFSET(SunWork, alphaScale, 0x8);
WWHD_OFFSET(SunWork, uvOne, 0xC);
WWHD_OFFSET(SunWork, input, 0x10);
WWHD_OFFSET(SunWork, color, 0x1C);
WWHD_OFFSET(SunWork, output, 0x20);
WWHD_OFFSET(SunWork, sun, 0x2C);
WWHD_OFFSET(SunWork, secondary, 0x38);
WWHD_OFFSET(SunWork, quad, 0x3C);
WWHD_OFFSET(SunWork, moon, 0x78);
WWHD_OFFSET(SunWork, stage, 0x84);
WWHD_OFFSET(SunWork, current, 0x8C);
WWHD_OFFSET(SunWork, relative, 0x94);
WWHD_OFFSET(SunWork, cameraDirection, 0xA0);
WWHD_OFFSET(SunWork, sunDirection, 0xAC);
WWHD_OFFSET(SunWork, matrix, 0xB8);
WWHD_OFFSET(SunWork, rotation, 0xE8);
WWHD_OFFSET(SunWork, dayScales, 0x118);
WWHD_SIZE(SunWork, 0x134);

struct CloudRenderState {
    /* 0x0 */ be<u8> enabled;
    /* 0x1 */ be<u8> secondaryEnabled;
    u8 padding2[0xA];
    /* 0xC */ be<u32> blendMode;
    /* 0x10 */ be<u32> sourceColorFactor;
    /* 0x14 */ be<u32> destinationColorFactor;
    /* 0x18 */ be<u32> sourceAlphaFactor;
    /* 0x1C */ be<u32> destinationAlphaFactor;
    /* 0x20 */ be<u32> depthTest;
    /* 0x24 */ be<u32> depthWrite;
    u8 padding28[0xB8];
    /* 0xE0 */ be<u8> clipEnabled;
    u8 paddingE1[0xB];
    /* 0xEC */ be<u32> cullMode;
    u8 tail[0x2C];
};
WWHD_OFFSET(CloudRenderState, enabled, 0x0);
WWHD_OFFSET(CloudRenderState, secondaryEnabled, 0x1);
WWHD_OFFSET(CloudRenderState, blendMode, 0xC);
WWHD_OFFSET(CloudRenderState, sourceColorFactor, 0x10);
WWHD_OFFSET(CloudRenderState, destinationColorFactor, 0x14);
WWHD_OFFSET(CloudRenderState, sourceAlphaFactor, 0x18);
WWHD_OFFSET(CloudRenderState, destinationAlphaFactor, 0x1C);
WWHD_OFFSET(CloudRenderState, depthTest, 0x20);
WWHD_OFFSET(CloudRenderState, depthWrite, 0x24);
WWHD_OFFSET(CloudRenderState, clipEnabled, 0xE0);
WWHD_OFFSET(CloudRenderState, cullMode, 0xEC);
WWHD_SIZE(CloudRenderState, 0x11C);
struct Projection {
    /* 0x0 */ be<u8> dirty;
    u8 padding1[0x93];
    /* 0x94 */ be<f32> nearPlane;
    /* 0x98 */ be<f32> farPlane;
    u8 padding9C[0x14];
    /* 0xB0 */ be<f32> viewportWidth;
    /* 0xB4 */ be<f32> viewportHeight;
    u8 tail[0x8];
};
WWHD_OFFSET(Projection, dirty, 0x0);
WWHD_OFFSET(Projection, nearPlane, 0x94);
WWHD_OFFSET(Projection, farPlane, 0x98);
WWHD_OFFSET(Projection, viewportWidth, 0xB0);
WWHD_OFFSET(Projection, viewportHeight, 0xB4);
WWHD_SIZE(Projection, 0xC0);
struct CloudWork {
    u8 padding0[0x18];
    /* 0x18 */ be<f32> projectionScale;
    u8 padding1C[0x4];
    /* 0x20 */ be<f32> minimumDrawAlpha;
    u8 padding24[0x4];
    /* 0x28 */ be<f32> minimumX;
    /* 0x2C */ be<f32> maximumX;
    /* 0x30 */ be<f32> loopScale;
    /* 0x34 */ be<f32> minimumY;
    /* 0x38 */ be<f32> maximumY;
    /* 0x3C */ be<f32> minimumAlpha;
    u8 padding40[0x4];
    /* 0x44 */ be<f32> sizeGain;
    /* 0x48 */ be<f32> bank;
    /* 0x4C */ be<f32> layerBlendOffset;
    u8 padding50[0x28];
    /* 0x78 */ Quad vertices;
    /* 0xA8 */ Vector projectedCenter;
    /* 0xB4 */ ColorF color;
    /* 0xC4 */ Vector projectedParticle;
    /* 0xD0 */ Vector projectedSun;
    /* 0xDC */ Vector worldPosition;
    /* 0xE8 */ Vector projectInput;
    /* 0xF4 */ Vector centerInput;
    /* 0x100 */ Vector center;
    /* 0x10C */ Vector centerDifference;
    /* 0x118 */ Vector topCenter;
    /* 0x124 */ Vector topDifference;
    /* 0x130 */ Vector bottomCenter;
    /* 0x13C */ Vector bottomDifference;
    /* 0x148 */ ColorF centerColor;
    /* 0x158 */ ColorF scaledDifference;
    /* 0x168 */ ColorF colorDifference;
    /* 0x178 */ ColorF highlightColor;
    /* 0x188 */ ColorF cloudColor;
    /* 0x198 */ ProjectParams particleProjection;
    /* 0x1C0 */ SafeString stageName;
    /* 0x1C8 */ Color packedCloudColor;
    /* 0x1CC */ Color packedCenterColor;
    /* 0x1D0 */ ProjectParams sunProjection;
    /* 0x1F8 */ CloudRenderState renderState;
    /* 0x314 */ SafeString currentStage;
    /* 0x31C */ SafeString farStage;
    /* 0x324 */ Matrix view;
    /* 0x354 */ Matrix rotation;
    /* 0x384 */ ColorF floatColor;
    /* 0x394 */ Projection projection;
    u8 padding454[0x4];
    /* 0x458 */ be<u32> projectionMatrix[16];
    /* 0x498 */ Matrix drawMatrix;
};
WWHD_OFFSET(CloudWork, projectionScale, 0x18);
WWHD_OFFSET(CloudWork, minimumDrawAlpha, 0x20);
WWHD_OFFSET(CloudWork, minimumX, 0x28);
WWHD_OFFSET(CloudWork, maximumX, 0x2C);
WWHD_OFFSET(CloudWork, loopScale, 0x30);
WWHD_OFFSET(CloudWork, minimumY, 0x34);
WWHD_OFFSET(CloudWork, maximumY, 0x38);
WWHD_OFFSET(CloudWork, minimumAlpha, 0x3C);
WWHD_OFFSET(CloudWork, sizeGain, 0x44);
WWHD_OFFSET(CloudWork, bank, 0x48);
WWHD_OFFSET(CloudWork, layerBlendOffset, 0x4C);
WWHD_OFFSET(CloudWork, vertices, 0x78);
WWHD_OFFSET(CloudWork, projectedCenter, 0xA8);
WWHD_OFFSET(CloudWork, color, 0xB4);
WWHD_OFFSET(CloudWork, projectedParticle, 0xC4);
WWHD_OFFSET(CloudWork, projectedSun, 0xD0);
WWHD_OFFSET(CloudWork, worldPosition, 0xDC);
WWHD_OFFSET(CloudWork, projectInput, 0xE8);
WWHD_OFFSET(CloudWork, centerInput, 0xF4);
WWHD_OFFSET(CloudWork, center, 0x100);
WWHD_OFFSET(CloudWork, centerDifference, 0x10C);
WWHD_OFFSET(CloudWork, topCenter, 0x118);
WWHD_OFFSET(CloudWork, topDifference, 0x124);
WWHD_OFFSET(CloudWork, bottomCenter, 0x130);
WWHD_OFFSET(CloudWork, bottomDifference, 0x13C);
WWHD_OFFSET(CloudWork, centerColor, 0x148);
WWHD_OFFSET(CloudWork, scaledDifference, 0x158);
WWHD_OFFSET(CloudWork, colorDifference, 0x168);
WWHD_OFFSET(CloudWork, highlightColor, 0x178);
WWHD_OFFSET(CloudWork, cloudColor, 0x188);
WWHD_OFFSET(CloudWork, particleProjection, 0x198);
WWHD_OFFSET(CloudWork, stageName, 0x1C0);
WWHD_OFFSET(CloudWork, packedCloudColor, 0x1C8);
WWHD_OFFSET(CloudWork, packedCenterColor, 0x1CC);
WWHD_OFFSET(CloudWork, sunProjection, 0x1D0);
WWHD_OFFSET(CloudWork, renderState, 0x1F8);
WWHD_OFFSET(CloudWork, currentStage, 0x314);
WWHD_OFFSET(CloudWork, farStage, 0x31C);
WWHD_OFFSET(CloudWork, view, 0x324);
WWHD_OFFSET(CloudWork, rotation, 0x354);
WWHD_OFFSET(CloudWork, floatColor, 0x384);
WWHD_OFFSET(CloudWork, projection, 0x394);
WWHD_OFFSET(CloudWork, projectionMatrix, 0x458);
WWHD_OFFSET(CloudWork, drawMatrix, 0x498);
WWHD_SIZE(CloudWork, 0x4C8);

struct PolyPass {
    be<u32> __vtbl;
    be<u8> flags[7];
    u8 padding;
};
struct GroundCheck {
    cBgS_GndChk base;
    PolyPass pass;
    dBgS_GrpPassChk group;
};
// HD: the original 160-byte workspace contains a constructed 84-byte derived ground query.
struct HousiWork {
    Vector position, velocity, eye, pointWind, wind;
    u8 reserved[12];
    GroundCheck ground;
    u8 tail[4];
};
struct SinCos {
    be<f32> sine, cosine;
};
WWHD_SIZE(Vector, 12);
WWHD_SIZE(HOUSI_EFF, 0x50);
WWHD_OFFSET(HousiPacket, field_0x5ddc, 0x5E64);
WWHD_OFFSET(HousiPacket, mCount, 0x5E68);
WWHD_SIZE(GroundCheck, 84);
WWHD_OFFSET(HousiWork, ground, 0x48);
WWHD_SIZE(HousiWork, 160);
template <class... Args> inline u32 getEnvironment(Args... args) {
    return gabi::call<u32>(0x02555D0C, args...);
}
template <class... Args> inline u32 getGameInfo(Args... args) { return gabi::call<u32>(0x025200D4, args...); }
template <class... Args> inline void dKyw_get_wind_vecpow(Args... args) {
    return gabi::call<void>(0x0257DB28, args...);
}
template <class... Args> inline void constructGroundCheck(Args... args) {
    return gabi::call<void>(0x02008E0C, args...);
}
template <class... Args> inline void cLib_addCalc(Args... args) {
    return gabi::call<void>(0x0200ECD4, args...);
}
template <class... Args> inline void dKy_set_eyevect_calc2(Args... args) {
    return gabi::call<void>(0x0256401C, args...);
}
template <class... Args> inline f32 dKyw_get_wind_pow(Args... args) {
    return gabi::call<f32>(0x02578348, args...);
}
template <class... Args> inline f32 cM_rndF(Args... args) { return gabi::call<f32>(0x020198D8, args...); }
template <class... Args> inline f32 cM_rndFX(Args... args) { return gabi::call<f32>(0x02019918, args...); }
template <class... Args> inline u32 cM_rad2s(Args... args) { return gabi::call<u32>(0x02019510, args...); }
template <class... Args> inline void dKyw_pntwind_get_vecpow(Args... args) {
    return gabi::call<void>(0x0257E128, args...);
}
template <class... Args> inline f32 PSVECSquareDistance(Args... args) {
    return gabi::call<f32>(0x028E8DE8, args...);
}
template <class... Args> inline f32 sqrtFloatHD(Args... args) { return gabi::call<f32>(0x028F4384, args...); }
template <class... Args> inline void get_vectle_calc(Args... args) {
    return gabi::call<void>(0x02563F0C, args...);
}
template <class... Args> inline void destroyGroundCheck(Args... args) {
    return gabi::call<void>(0x02008DAC, args...);
}
// HD: TU data and GHS vtable identities; guest global views create no duplicate statics.
enum : u32 {
    Zero = 0x1004F528,
    One = 0x1004F550,
    Rate = 0x1004F554,
    Increment = 0x1004F580,
    AlphaStep = 0x1004F590,
    EyeDistance = 0x1004F6D0,
    Drag = 0x1004F588,
    Fall = 0x1004F740,
    Degrees = 0x1004F6D8,
    WindScale = 0x1004F730,
    VelocityLimit = 0x1004F728,
    MotionStep = 0x1004F734,
    RotationStep = 0x1004F73C,
    WindForce = 0x1004F72C,
    Radius = 0x1004F720,
    Half = 0x1004F57C,
    Floor = 0x1004F738,
    MinimumStep = 0x1004F724,
    InitialSpeed = 0x1004F744,
    FullAngle = 0x1004F748,
    LowestBase = 0x1004F74C,
    BaseResetHeight = 0x1004F750,
    BaseHeightOffset = 0x1004F6B8,
    RespawnRadius = 0x1004F754,
    RespawnJitter = 0x1004F564,
    SinCosTable = 0x104A44F8,
    GroundCheckVtable = 0x1004F3F4,
    PolyInfoVtable = 0x1004F404,
    GroupPassVtable = 0x1004F414,
    PolyPassVtable = 0x1004F424,
    GroupPassDestructorVtable = 0x1004F3E4
};
namespace sdk {
// 028E91EC [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R MTXInverse(A... args) {
    return gabi::call<R>(0x028E91EC, args...);
}
// 02563F68 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R dKy_set_eyevect_calc(A... args) {
    return gabi::call<R>(0x02563F68, args...);
}
// 02563F64 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R dKyr_get_vectle_calc(A... args) {
    return gabi::call<R>(0x02563F64, args...);
}
// 0257015C [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R convertColor(A... args) {
    return gabi::call<R>(0x0257015C, args...);
}
// 027FC500 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R setUniformColor(A... args) {
    return gabi::call<R>(0x027FC500, args...);
}
// 0257FB4C [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R drawSibukiPacket(A... args) {
    return gabi::call<R>(0x0257FB4C, args...);
}
// 027B5E94 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R flushBuffer(A... args) {
    return gabi::call<R>(0x027B5E94, args...);
}
// 02563DD8 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R prepareProjection(A... args) {
    return gabi::call<R>(0x02563DD8, args...);
}
// 025F1018 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R project(A... args) {
    return gabi::call<R>(0x025F1018, args...);
}
// 028E98C0 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R MTXRotDeg(A... args) {
    return gabi::call<R>(0x028E98C0, args...);
}
// 028E9108 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R MTXConcat(A... args) {
    return gabi::call<R>(0x028E9108, args...);
}
// 028E8F64 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R MTXMultVec(A... args) {
    return gabi::call<R>(0x028E8F64, args...);
}
// 028E8DE8 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R distanceSquared(A... args) {
    return gabi::call<R>(0x028E8DE8, args...);
}
// 028F4384 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R squareRoot(A... args) {
    return gabi::call<R>(0x028F4384, args...);
}
// 0201A478 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R constructAngle(A... args) {
    return gabi::call<R>(0x0201A478, args...);
}
// 0258115C [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R drawStarPacket(A... args) {
    return gabi::call<R>(0x0258115C, args...);
}
// 025602F0 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R getSeaColor(A... args) {
    return gabi::call<R>(0x025602F0, args...);
}
// 0255F8A0 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R setSeaFog(A... args) {
    return gabi::call<R>(0x0255F8A0, args...);
}
// 02582518 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R prepareWaveColor(A... args) {
    return gabi::call<R>(0x02582518, args...);
}
// 02582650 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R drawWavePacket(A... args) {
    return gabi::call<R>(0x02582650, args...);
}
// 028F43F8 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R sine(A... args) { return gabi::call<R>(0x028F43F8, args...); }
// 0200ECD4 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R addCalc(A... args) {
    return gabi::call<R>(0x0200ECD4, args...);
}
// 020198D8 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R randomF(A... args) {
    return gabi::call<R>(0x020198D8, args...);
}
// 02019918 [v d_kankyo_rain_drawSibuki]
template <class R = void, class... A> inline R randomFX(A... args) {
    return gabi::call<R>(0x02019918, args...);
}
} // namespace sdk
enum : u32 {
    BankToRadians = 0x1004F5F8,
    SkyRange = 0x1004F52C,
    SprayHeight = 0x1004F53C,
    SprayMinimumStep = 0x1004F58C,
    SprayForward = 0x1004F5B0,
    DirectionBias = 0x1004F844,
    NarrowFovy = 0x1004F6D4,
    SprayYRange = 0x1004F7D0,
    SprayPositionRange = 0x1004FC68,
    SprayPositionScale = 0x1004FC64,
    TextureEdge = 0x1004F7FC,
    DegreeToRadians = 0x1004FB7C,
    WideFovy = 0x1004F6EC,
    StarSizeBase = 0x1004FC98,
    MoonDistanceGain = 0x1004F6BC,
    MoonDistanceBase = 0x1004F55C,
    ScreenMinimum = 0x1004FCAC,
    SpiralShrink = 0x1004FCB0,
    ScreenMaximumX = 0x1004F634,
    ScreenMaximumY = 0x1004F7CC,
    SpiralDistanceGain = 0x1004F800,
    StarTableScale = 0x1004F54C,
    LargeStarScale = 0x1004FCB4,
    StarVariation = 0x1004FCB8,
    StarVariationScale = 0x1004F640,
    LargeStarGain = 0x1004FB8C,
    SpiralHeight = 0x1004FB84,
    SpiralResetScale = 0x1004F540,
    WaveStageName = 0x1004FCC0,
    SafeStringVtableAddress = 0x1004F3AC,
    WaveFadeScale = 0x1004FCBC,
    ByteAlpha = 0x1004F5AC,
    ScalarOne = 0x1004F5D0,
    WideTexture = 0x1004F8D8,
    WaveWidthScale = 0x1004F8D4,
    ConstellationInitialized = 0x10477588,
    ConstellationPositions = 0x10477520,
    WaveOffsetsInitialized = 0x1047758C,
    WaveOffsets = 0x10477480
};
struct SafeStringVtable {
    u8 prefix[0x14];
    be<u32> callback;
};
inline void safe_string_callback(SafeString *string) {
    gabi::call_ptr(at<SafeStringVtable>(string->vtable)->callback, string);
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
inline f32 wave_multiply_double(f32 value, f64 sine) {
    u64 bits = std::bit_cast<u64>(sine);
    bits = (bits & 0xFFFFFFFFF8000000ull) + (bits & 0x08000000ull);
    return (f32)((f64)value * std::bit_cast<f64>(bits));
}
inline BufferSlot *bufferSlot(u32 geometry, u32 selected) {
    return at<BufferSlot>(geometry + selected * u32(sizeof(BufferSlot)));
}
struct DrawInfo {
    u8 prefix[12];
    be<u32> mode;
};

enum : u32 {
    SunStageName = 0x1004FB9C,
    MoonTimeEnd = 0x1004FB74,
    MoonTimeBegin = 0x1004FB78,
    SaveInfoPointer = 0x101F84DC,
    MoonDayThreshold = 0x1004F5DC,
    MoonFlip = 0x1004FAEC,
    MoonDayScaleTable = 0x1004FBA4,
    MoonAngleScale = 0x1004FB80,
    MoonAlphaScale = 0x1004FB88,
    SunAngleOffset = 0x1004FB90,
    SunSize = 0x1004FB94,
    SunGrowth = 0x1004F5B4,
    SunAlphaScale = 0x1004FB98
};
namespace sun_sdk {
// 02555D0C [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R environment(A... args) {
    return gabi::call<R>(0x02555D0C, args...);
}
// 025200D4 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R gameInfo(A... args) {
    return gabi::call<R>(0x025200D4, args...);
}
// 02554288 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R moonDay(A... args) {
    return gabi::call<R>(0x02554288, args...);
}
// 028E91EC [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R inverse(A... args) {
    return gabi::call<R>(0x028E91EC, args...);
}
// 0256F084 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R preparePosition(A... args) {
    return gabi::call<R>(0x0256F084, args...);
}
// 02563F64 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R direction(A... args) {
    return gabi::call<R>(0x02563F64, args...);
}
// 028F4384 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R squareRoot(A... args) {
    return gabi::call<R>(0x028F4384, args...);
}
// 028F4D28 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R atan2(A... args) { return gabi::call<R>(0x028F4D28, args...); }
// 028E98C0 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R rotate(A... args) {
    return gabi::call<R>(0x028E98C0, args...);
}
// 028E9108 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R concat(A... args) {
    return gabi::call<R>(0x028E9108, args...);
}
// 0257E8C0 [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R setSunColor(A... args) {
    return gabi::call<R>(0x0257E8C0, args...);
}
// 0257E9DC [v d_kankyo_rain_drawSun]
template <class R = void, class... A> inline R drawSunPacket(A... args) {
    return gabi::call<R>(0x0257E9DC, args...);
}
} // namespace sun_sdk
struct StageInfo {
    u8 prefix[12];
    be<u32> flags;
};
struct StageVtable {
    u8 prefix[0x15C];
    be<u32> getInfo;
};
struct SaveTime {
    u8 prefix[0x44];
    be<f32> time;
};
struct SunVertex {
    Vector position;
    u8 materialData[0x2C];
    be<f32> u, v;
    u8 tail[0x58];
};
WWHD_SIZE(SunVertex, 0x98);

inline bool sun_stage(u32 name, u32 left, u32 right) {
    auto a = at<SafeString>(left), b = at<SafeString>(right);
    a->text = name;
    a->vtable = SafeStringVtableAddress;
    u32 game = getGameInfo();
    b->text = ea(&at<GameInfo>(game)->mStageName);
    b->vtable = SafeStringVtableAddress;
    safe_string_callback(a);
    safe_string_callback(a);
    u32 first = a->text;
    safe_string_callback(b);
    u32 second = b->text;
    return equal_terminated_strings(first, second);
}
inline void sun_vertex(SunWork *work, u32 matrix, u32 base, u32 output) {
    sdk::MTXMultVec(at<void>(matrix), &work->input, &work->output);
    auto origin = at<Vector>(base), vertex = at<Vector>(output);
    vertex->channels[0] = fadds_ppc((f32)origin->channels[0], (f32)work->output.channels[0]);
    vertex->channels[1] = fadds_ppc((f32)origin->channels[1], (f32)work->output.channels[1]);
    vertex->channels[2] = fadds_ppc((f32)origin->channels[2], (f32)work->output.channels[2]);
}
inline void sun_quad(u32 manager, SunWork *work, f32 zero, f32 one) {
    auto geometry = at<Geometry>(manager);
    u32 index = geometry->selected;
    u32 buffer = bufferSlot(manager, index)->buffer, end = buffer + 0x260;
    for (u32 cursor = buffer; cursor < end; cursor += 32)
        for (u32 j = 0; j < 32; j++)
            at<CacheLine>(cursor & ~31u)->bytes[j] = 0;
    index = geometry->selected;
    buffer = bufferSlot(manager, index)->buffer;
    for (u32 i = 0; i < 4; i++) {
        auto vertex = at<SunVertex>(buffer + i * u32(sizeof(SunVertex)));
        vertex->position.words[0] = (u32)work->quad.words[i][0];
        vertex->position.words[1] = (u32)work->quad.words[i][1];
        vertex->position.words[2] = (u32)work->quad.words[i][2];
        vertex->u = (i == 1 || i == 2) ? one : zero;
        vertex->v = i >= 2 ? one : zero;
    }
}

struct CloudStageInfo {
    u8 padding0[0x4];
    /* 0x4 */ be<f32> farPlane;
};
WWHD_OFFSET(CloudStageInfo, farPlane, 0x4);
struct ShaderCache {
    /* 0x0 */ be<u32> shader;
    /* 0x4 */ be<u32> material;
};
WWHD_OFFSET(ShaderCache, shader, 0x0);
WWHD_OFFSET(ShaderCache, material, 0x4);
WWHD_SIZE(ShaderCache, 0x8);
struct Material {
    /* 0x0 */ be<u8> flags;
    u8 padding1[0x3];
    /* 0x4 */ be<u32> data;
    u8 padding8[0x4];
    /* 0xC */ be<u32> size;
    u8 padding10[0x6C];
    /* 0x7C */ be<u32> shaderResource;
};
WWHD_OFFSET(Material, flags, 0x0);
WWHD_OFFSET(Material, data, 0x4);
WWHD_OFFSET(Material, size, 0xC);
WWHD_OFFSET(Material, shaderResource, 0x7C);
struct ShaderResource {
    u8 padding0[0x28];
    /* 0x28 */ be<u32> shader;
};
WWHD_OFFSET(ShaderResource, shader, 0x28);
struct CloudResource {
    /* 0x0 */ be<u32> material;
    u8 padding4[0x8];
    /* 0xC */ be<u32> bindingCount;
    /* 0x10 */ be<u32> bindings;
    /* 0x14 */ be<u32> textureCount;
    /* 0x18 */ be<u32> textures;
};
WWHD_OFFSET(CloudResource, material, 0x0);
WWHD_OFFSET(CloudResource, bindingCount, 0xC);
WWHD_OFFSET(CloudResource, bindings, 0x10);
WWHD_OFFSET(CloudResource, textureCount, 0x14);
WWHD_OFFSET(CloudResource, textures, 0x18);
struct ShaderBindings {
    u8 padding0[0xC];
    /* 0xC */ be<s16> vertex;
    /* 0xE */ be<s16> pixel;
    /* 0x10 */ be<s16> geometry;
};
WWHD_OFFSET(ShaderBindings, vertex, 0xC);
WWHD_OFFSET(ShaderBindings, pixel, 0xE);
WWHD_OFFSET(ShaderBindings, geometry, 0x10);
struct ShaderUniform {
    u8 padding0[0x4];
    /* 0x4 */ be<u32> data;
    u8 padding8[0x4];
    /* 0xC */ be<u32> size;
    u8 tail[0xC];
};
WWHD_OFFSET(ShaderUniform, data, 0x4);
WWHD_OFFSET(ShaderUniform, size, 0xC);
WWHD_SIZE(ShaderUniform, 0x1C);
struct CloudShaderState {
    u8 padding0[0x10];
    /* 0x10 */ ShaderUniform uniforms[2];
    u8 padding48[0x4];
    /* 0x4C */ be<u32> selected;
    u8 padding50[0x194];
    /* 0x1E4 */ be<u32> color[4];
    /* 0x1F4 */ be<f32> scaleX;
    /* 0x1F8 */ be<f32> scaleY;
};
WWHD_OFFSET(CloudShaderState, uniforms, 0x10);
WWHD_OFFSET(CloudShaderState, selected, 0x4C);
WWHD_OFFSET(CloudShaderState, color, 0x1E4);
WWHD_OFFSET(CloudShaderState, scaleX, 0x1F4);
WWHD_OFFSET(CloudShaderState, scaleY, 0x1F8);
struct CloudSetup {
    u8 padding0[0x4];
    /* 0x4 */ be<u32> state;
    u8 padding8[4];
    /* 0xC */ u8 uniformObject[12];
    /* 0x18 */ be<u32> vtable;
};
WWHD_OFFSET(CloudSetup, state, 0x4);
WWHD_OFFSET(CloudSetup, uniformObject, 0xC);
WWHD_OFFSET(CloudSetup, vtable, 0x18);
struct UniformVtable {
    u8 padding0[0x2C];
    /* 0x2C */ be<u32> apply;
};
WWHD_OFFSET(UniformVtable, apply, 0x2C);
struct CloudUniformView {
    u8 padding0[0xC];
    /* 0xC */ be<u32> vtable;
};
WWHD_OFFSET(CloudUniformView, vtable, 0xC);
struct Renderer {
    u8 padding0[0x148];
    /* 0x148 */ be<u32> projectionWords[17];
    /* 0x18C */ u8 renderState[1];
};
WWHD_OFFSET(Renderer, projectionWords, 0x148);
WWHD_OFFSET(Renderer, renderState, 0x18C);
struct RenderGlobal {
    u8 padding0[0x1020];
    /* 0x1020 */ be<u32> count;
    /* 0x1024 */ be<u32> views;
};
WWHD_OFFSET(RenderGlobal, count, 0x1020);
WWHD_OFFSET(RenderGlobal, views, 0x1024);
struct ActiveView {
    u8 padding0[0x4C];
    /* 0x4C */ be<u32> view;
    /* 0x50 */ be<u32> flags;
    u8 padding54[0x110];
    /* 0x164 */ be<u32> alternativeView;
};
WWHD_OFFSET(ActiveView, view, 0x4C);
WWHD_OFFSET(ActiveView, flags, 0x50);
WWHD_OFFSET(ActiveView, alternativeView, 0x164);
struct CloudView {
    u8 padding0[0x90];
    /* 0x90 */ be<u32> vtable;
    /* 0x94 */ be<f32> left;
    /* 0x98 */ be<f32> right;
    /* 0x9C */ be<f32> top;
    u8 paddingA0[0xC];
    /* 0xAC */ be<f32> bottom;
    /* 0xB0 */ be<f32> width;
    /* 0xB4 */ be<f32> height;
    /* 0xB8 */ u8 matrix[64];
};
WWHD_OFFSET(CloudView, vtable, 0x90);
WWHD_OFFSET(CloudView, left, 0x94);
WWHD_OFFSET(CloudView, right, 0x98);
WWHD_OFFSET(CloudView, top, 0x9C);
WWHD_OFFSET(CloudView, bottom, 0xAC);
WWHD_OFFSET(CloudView, width, 0xB0);
WWHD_OFFSET(CloudView, height, 0xB4);
WWHD_OFFSET(CloudView, matrix, 0xB8);
struct CloudViewVtable {
    u8 padding0[0xC];
    /* 0xC */ be<u32> hasMatrix;
};
WWHD_OFFSET(CloudViewVtable, hasMatrix, 0xC);
struct CloudDraw {
    /* 0x0 */ be<u32> primitive;
    /* 0x4 */ be<u32> count;
    /* 0x8 */ be<u32> indexType;
    /* 0xC */ be<u32> indices;
};
WWHD_OFFSET(CloudDraw, primitive, 0x0);
WWHD_OFFSET(CloudDraw, count, 0x4);
WWHD_OFFSET(CloudDraw, indexType, 0x8);
WWHD_OFFSET(CloudDraw, indices, 0xC);
WWHD_SIZE(CloudDraw, 0x10);
enum : u32 {
    CloudSpecialStage = 0x1004FD08,
    CloudFarStage = 0x1004FD10,
    CloudZero = 0x10145180,
    CloudOne = 0x1014517C,
    FarPlaneInset = 0x1004F594,
    CloudFarPlane = 0x1004FCC8,
    CloudRenderer = 0x104B45C0,
    RenderGlobalPointer = 0x101F95D0,
    CloudLoopScale = 0x1004F5E8,
    DefaultCloudView = 0x104A20FC,
    ProjectionInitialized = 0x101FD9F0,
    ProjectionVtablePointer = 0x101FDCD4,
    ProjectionVtableAddress = 0x1004F4E4,
    CloudProjectionFar = 0x1004FCCC,
    CloudColorWords = 0x104A01CC,
    CloudUniformScale = 0x1004FCD0,
    CloudSizeBase = 0x1004FCD8,
    CloudSizeGain = 0x1004FCDC,
    CloudPitchLimit = 0x1004FCD4,
    CloudMinimumDrawAlpha = 0x1004F5B8,
    CloudMinimumAlpha = 0x1004FCE0,
    CloudMaximumX = 0x1004FCEC,
    CloudMinimumY = 0x1004FCE8,
    CloudMaximumY = 0x1004FCE4,
    CloudMinimumX = 0x1004FCF0,
    CloudProjectionScale = 0x1004FCF4,
    CloudVariation = 0x1004FCF8,
    CloudSmallOffset = 0x1004F544,
    CloudPitchScale = 0x1004FCFC,
    CloudPitchGain = 0x1004FD04,
    CloudSideGain = 0x1004FD00,
    CloudFadeDistance = 0x1004F558,
    CloudDrawAddress = 0x104B4CFC
};
namespace cloud_sdk {
// 0257015C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R convertColor(A... args) {
    return gabi::call<R>(0x0257015C, args...);
}
// 0274D458 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R scaleColor(A... args) {
    return gabi::call<R>(0x0274D458, args...);
}
// 02750250 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R constructRenderState(A... args) {
    return gabi::call<R>(0x02750250, args...);
}
// 02563DD8 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R prepareProjection(A... args) {
    return gabi::call<R>(0x02563DD8, args...);
}
// 025F1018 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R project(A... args) {
    return gabi::call<R>(0x025F1018, args...);
}
// 028E91EC [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R inverse(A... args) {
    return gabi::call<R>(0x028E91EC, args...);
}
// 027F29D4 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R shaderCache(A... args) {
    return gabi::call<R>(0x027F29D4, args...);
}
// 027BB9E0 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R updateMaterial(A... args) {
    return gabi::call<R>(0x027BB9E0, args...);
}
// 027B9F68 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setShader(A... args) {
    return gabi::call<R>(0x027B9F68, args...);
}
// C00060E0 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setMaterialData(A... args) {
    return gabi::call<R>(0xC00060E0, args...);
}
// 027BB7CC [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R applyMaterial(A... args) {
    return gabi::call<R>(0x027BB7CC, args...);
}
// 027389F8 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R constructProjection(A... args) {
    return gabi::call<R>(0x027389F8, args...);
}
// 0274E04C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setProjectionBounds(A... args) {
    return gabi::call<R>(0x0274E04C, args...);
}
// 02738AC0 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setProjectionMatrix(A... args) {
    return gabi::call<R>(0x02738AC0, args...);
}
// 0274D83C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R projectionMatrix(A... args) {
    return gabi::call<R>(0x0274D83C, args...);
}
// 028E8970 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R copyMatrix(A... args) {
    return gabi::call<R>(0x028E8970, args...);
}
// 02738A6C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setProjectionMode(A... args) {
    return gabi::call<R>(0x02738A6C, args...);
}
// 02575ACC [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R drawMatrix(A... args) {
    return gabi::call<R>(0x02575ACC, args...);
}
// 027FDA54 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setupShader(A... args) {
    return gabi::call<R>(0x027FDA54, args...);
}
// 027FDFF4 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R selectUniform(A... args) {
    return gabi::call<R>(0x027FDFF4, args...);
}
// C0006900 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setPixelUniform(A... args) {
    return gabi::call<R>(0xC0006900, args...);
}
// C0006A38 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setVertexUniform(A... args) {
    return gabi::call<R>(0xC0006A38, args...);
}
// C00068A8 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setGeometryUniform(A... args) {
    return gabi::call<R>(0xC00068A8, args...);
}
// 027FB678 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R prepareUniform(A... args) {
    return gabi::call<R>(0x027FB678, args...);
}
// 027505B4 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R applyCullMode(A... args) {
    return gabi::call<R>(0x027505B4, args...);
}
// 027505DC [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R applyBlendMode(A... args) {
    return gabi::call<R>(0x027505DC, args...);
}
// 02750520 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R applyClipMode(A... args) {
    return gabi::call<R>(0x02750520, args...);
}
// 02750534 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R applyEnable(A... args) {
    return gabi::call<R>(0x02750534, args...);
}
// 028E98C0 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R rotate(A... args) {
    return gabi::call<R>(0x028E98C0, args...);
}
// 028E9108 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R concat(A... args) {
    return gabi::call<R>(0x028E9108, args...);
}
// 0201AD78 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R subtractVector(A... args) {
    return gabi::call<R>(0x0201AD78, args...);
}
// 02019510 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R angle(A... args) { return gabi::call<R>(0x02019510, args...); }
// 028F4384 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R squareRoot(A... args) {
    return gabi::call<R>(0x028F4384, args...);
}
// 028F4D28 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R atan2(A... args) { return gabi::call<R>(0x028F4D28, args...); }
// 028F4BE0 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R cosine(A... args) {
    return gabi::call<R>(0x028F4BE0, args...);
}
// 028F43F8 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R sine(A... args) { return gabi::call<R>(0x028F43F8, args...); }
// 028E8D88 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R addVector(A... args) {
    return gabi::call<R>(0x028E8D88, args...);
}
// 0201AE48 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R scaleVector(A... args) {
    return gabi::call<R>(0x0201AE48, args...);
}
// 028E8DE8 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R distanceSquared(A... args) {
    return gabi::call<R>(0x028E8DE8, args...);
}
// 0274D380 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R subtractColor(A... args) {
    return gabi::call<R>(0x0274D380, args...);
}
// 0274D314 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R addColor(A... args) {
    return gabi::call<R>(0x0274D314, args...);
}
// 027FC500 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R setUniformColor(A... args) {
    return gabi::call<R>(0x027FC500, args...);
}
// 027BE53C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R bindTexture(A... args) {
    return gabi::call<R>(0x027BE53C, args...);
}
// 027BFE5C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R prepareGeometry(A... args) {
    return gabi::call<R>(0x027BFE5C, args...);
}
// C0006178 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R drawIndexed(A... args) {
    return gabi::call<R>(0xC0006178, args...);
}
// 0255F84C [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R restoreFog(A... args) {
    return gabi::call<R>(0x0255F84C, args...);
}
// 027B5E94 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R flushBuffer(A... args) {
    return gabi::call<R>(0x027B5E94, args...);
}
// 02750370 [v d_kankyo_rain_drawVrkumo]
template <class R = void, class... A> inline R restoreRenderState(A... args) {
    return gabi::call<R>(0x02750370, args...);
}
} // namespace cloud_sdk
inline bool cloud_stage(u32 literal, u32 first, u32 second) {
    auto a = at<SafeString>(first), b = at<SafeString>(second);
    a->text = literal;
    a->vtable = SafeStringVtableAddress;
    b->text = ea(&at<GameInfo>(getGameInfo())->mStageName);
    b->vtable = SafeStringVtableAddress;
    gabi::call_ptr((u32)at<SafeStringVtable>(a->vtable)->callback, first);
    gabi::call_ptr((u32)at<SafeStringVtable>(a->vtable)->callback, first);
    u32 left = a->text;
    gabi::call_ptr((u32)at<SafeStringVtable>(b->vtable)->callback, second);
    u32 right = b->text;
    return left == right || equal_terminated_strings(left, right);
}
inline u32 cloud_stage_info() {
    u32 game = getGameInfo();
    return gabi::call_ptr<u32>((u32)at<StageVtable>(at<GameInfo>(game)->mStageVtable)->getInfo,
                               ea(&at<GameInfo>(game)->mStageVtable));
}

inline u32 cloud_setup_uniform(u32 setup) { return ea(&at<CloudSetup>(setup)->uniformObject); }

// HD: dynamic shader/view calls retain the original GHS vtable entries and integer argument ABI.
inline u32 cloud_has_matrix(u32 view) {
    return gabi::call_ptr<u32>((u32)at<CloudViewVtable>(at<CloudView>(view)->vtable)->hasMatrix, view);
}
inline void cloud_apply_setup(u32 setup, u32 resource) {
    gabi::call_ptr((u32)at<UniformVtable>(at<CloudSetup>(setup)->vtable)->apply, cloud_setup_uniform(setup),
                   resource);
}
inline void cloud_apply_uniform(u32 uniform, u32 resource) {
    gabi::call_ptr((u32)at<UniformVtable>(at<CloudUniformView>(uniform)->vtable)->apply, uniform, resource);
}
inline u32 sun_stage_info(u32 game) {
    return gabi::call_ptr<u32>((u32)at<StageVtable>(at<GameInfo>(game)->mStageVtable)->getInfo,
                               at<void>(ea(&at<GameInfo>(game)->mStageVtable)));
}
struct RendererProjection {
    u8 prefix[0x240];
    u8 shaderProjection[1];
};
struct TextureList {
    u8 header[4];
    u8 entries[1];
};

} // namespace RainDraw
